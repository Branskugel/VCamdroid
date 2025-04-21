package com.darusc.vcamdroid.networking

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class ConnectionManager(ipAddress: String, port: Int) : TCPConnection.Listener {

    private val tcpConn: TCPConnection
    private val udpConn: UDPConnection

    private var streamingEnabled = false

    init {
        tcpConn = TCPConnection(ipAddress, port, this)
        udpConn = UDPConnection(ipAddress, port)
    }

    fun sendVideoStreamFrame() {
        if(!streamingEnabled) {
            return
        }

        CoroutineScope(Dispatchers.IO).launch {
//            udpConn.send()
        }
    }

    override fun onBytesReceived(bytes: CharArray, size: Int) {
        val message = String(bytes, 0, size)
        when(message) {
            "streamstart" -> streamingEnabled = true
            "streamstop" -> streamingEnabled = false
        }
    }
}