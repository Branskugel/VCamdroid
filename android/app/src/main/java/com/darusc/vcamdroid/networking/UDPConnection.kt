package com.darusc.vcamdroid.networking

import android.util.Log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress

/**
 * Wrapper around a socket to manage the TCP connection.
 * Using it only to send the video stream. No packet receiving implementation.
 */
class UDPConnection(
    ipAddress: String,
    port: Int,
) {
    private lateinit var socket: DatagramSocket

    private val TAG = "VCamdroid"

    init {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                socket = DatagramSocket()
                socket.connect(InetAddress.getByName(ipAddress), port)

                Log.d(TAG, "UDP Connected to $ipAddress:$port")
            } catch (e: Exception) {
                Log.e(TAG, "UDP Connection to $ipAddress:$port failed!" + e.message)
            }
        }
    }

    fun send(bytes: ByteArray, size: Int) {
        CoroutineScope(Dispatchers.IO).launch {
            socket.send(DatagramPacket(bytes, size))
        }
    }

    fun close() {
        socket.close()
    }
}