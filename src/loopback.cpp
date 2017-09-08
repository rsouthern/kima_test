#include "loopback.h"
#include <iostream>

// This depends on OSC C++ library: https://github.com/MugenSAS/osc-cpp-qt
// Compiles with no warnings on my platform
#include <osc/OscAPI.h>
#include <osc/composer/OscMessageComposer.h>
#include <osc/reader/OscReader.h>
#include <osc/reader/OscMessage.h>
#include <osc/reader/types/OscFloat.h>

LoopBack::LoopBack(QObject *parent)
    : QObject(parent), m_inSocket(this), m_outSocket(this) 
{
    // Initialise the input socket
    m_inSocket.bind(QHostAddress::LocalHost, m_wekOutputPort);

    // Connect the an input message to the local function to process the message
    connect(&m_inSocket, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));

    // Fire up the timer for this class
    if (startTimer(1000) == 0) {
        std::cerr << "Could not start timer!\n";
    }
}

LoopBack::~LoopBack() {
}

/**
 */
void LoopBack::processPendingDatagrams() {
    while (m_inSocket.hasPendingDatagrams()) {
        // Create a datagram packet and read it from the socket
        QByteArray datagram;
        datagram.resize(m_inSocket.pendingDatagramSize());   
        m_inSocket.readDatagram(datagram.data(), datagram.size());
        
        // Use the libosc to read and unpack the datagram packet
        OscReader reader(&datagram);
        OscMessage* msg = reader.getMessage();
        
        // Note: I'm not quite sure how to figure out how big the message is currently, so just reading the
        // address and the first value from the packet
        std::cerr << "Received " << datagram.size() << " bytes from port "<<m_wekOutputPort<<" ("<<msg->getAddress().toStdString() << ","<<msg->getValue( 0 )->toFloat()<<")\n";

        // This memory is allocated apparently, so needs to be deleted
        delete msg;	

        /*
        // This is a test of just passing floats around
        QByteArray datagram;

        // Retrieve the size of the incoming data
        size_t dataSize = m_inSocket.pendingDatagramSize();

        // Resize the byte array according to the size of the packet
        datagram.resize(dataSize);

        // Figure out how many floats were sent
        size_t msgDataCnt = dataSize / sizeof(float);

        // Retrieve the data into the byte array
        m_inSocket.readDatagram(datagram.data(), datagram.size());

        QDataStream s(&datagram, QIODevice::ReadOnly);
        float *msgData = new float[msgDataCnt];
        for (size_t i = 0; i<msgDataCnt; ++i) 
            s >> msgData[i];

        std::cout << "received "<<dataSize<< " bytes: ";
        std::copy(msgData, msgData + msgDataCnt, std::ostream_iterator<float>(std::cout, " "));
        std::cout << "\n";

        delete [] msgData;
        */
    }
}

/**
  * Use a QObject::timeEvent to fire off UDF packets
  */
void LoopBack::timerEvent(QTimerEvent *event) {
    // Start composing the message
    OscMessageComposer msg( "/wek/inputs");

    // Pack some data in the message (how many inputs would you like?)
    size_t msgSize = 5;
    for (size_t i = 0; i < msgSize; ++i) 
        msg.pushFloat(float(i));

    // This is packed into a QByteArray already, so getBytes is all we need to send
    if (m_outSocket.writeDatagram(*(msg.getBytes()), QHostAddress::LocalHost, m_wekInputPort) == -1) {
        // An error occurred - pick it up and display here
        std::cerr << "Socket Write Error: "<<m_outSocket.errorString().toStdString();
    } else {
        std::cerr << "LoopBack::timerEvent() - sending "<<msgSize<<" floats to port "<<m_wekInputPort<<'\n';
    }

    // Continue with regular processing of timer events where necessary
    return QObject::timerEvent(event);
    /*
    // Below is a simple test where floats are shunted around
    size_t msgDataCnt = 8;
    QByteArray datagram;

    datagram.resize(sizeof(float) * msgDataCnt);
    std::cout << "message size = " << datagram.size() << "\n";

    QDataStream s_out(&datagram, QIODevice::WriteOnly);
    if (false) s_out.setByteOrder(QDataStream::LittleEndian); // This may be needed on some systems
    for (size_t i = 0; i < msgDataCnt; ++i) {
        float tmp = float(i) * float(i);
        s_out << tmp;
    }

    if (m_outSocket.writeDatagram(datagram, QHostAddress::LocalHost, 6448) == -1) {
        // An error occurred - pick it up and display here
        std::cerr << "Socket Write Error: "<<m_outSocket.errorString().toStdString();
    }
    return QObject::timerEvent(event);
    */
}