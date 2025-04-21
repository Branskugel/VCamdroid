package com.darusc.vcamdroid.networking

import android.util.Log
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress
import java.net.SocketAddress
import kotlin.math.ceil
import kotlin.math.min

/**
 * Wrapper around a socket to manage the TCP connection.
 * Using it only to send the video stream. No packet receiving implementation.
 */
class UDPConnection(
    ipAddress: String,
    private val port: Int,
) {
    private lateinit var socket: DatagramSocket

    private val TAG = "VCamdroid"
    private val MAX_PACKET_SIZE = 65472.0

    private val address: InetAddress = InetAddress.getByName(ipAddress)

    init {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                socket = DatagramSocket()
                socket.connect(address, port)
                socket.sendBufferSize = 921600

                Log.d(TAG, "UDP Connected to $ipAddress:$port")
            } catch (e: Exception) {
                Log.e(TAG, "UDP Connection to $ipAddress:$port failed!" + e.message)
            }
        }
    }

    fun send(bytes: ByteArray, size: Int) {
//        CoroutineScope(Dispatchers.IO).launch {
            var segments = ceil(bytes.size / MAX_PACKET_SIZE).toInt()
            //println("Frame size ${bytes.size} split in $segments segments")
            var start = 0
            while(segments != 0) {
                val end = min(bytes.size, start + MAX_PACKET_SIZE.toInt())
                val chunk = bytes.copyOfRange(start, end)

                socket.send(DatagramPacket(chunk, chunk.size, address, port))

                start = end
                segments--

                socket.receive(DatagramPacket(ByteArray(5), 5, address, port))
                //println("Segments remaining $segments. Size of current segment ${chunk.size}")
//            }
        }
    }



    fun close() {
        socket.close()
    }
}