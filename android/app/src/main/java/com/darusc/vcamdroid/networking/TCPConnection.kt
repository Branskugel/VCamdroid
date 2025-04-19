package com.darusc.vcamdroid.networking

import android.util.Log
import java.io.BufferedReader
import java.io.InputStreamReader
import java.io.PrintWriter
import java.net.Socket
import java.net.SocketException
import java.util.concurrent.atomic.AtomicBoolean

/**
 * Wrapper around a socket to manage the TCP connection
 * @param ipAddress Remote endpoint's IPv4 address
 * @param port Remote endpoint's port
 * @param listener The registered listener for callbacks
 */
class TCPConnection(
    ipAddress: String,
    port: Int,
    private val listener: Listener
) {

    interface Listener {
        fun onBytesReceived(bytes: CharArray)
    }

    private lateinit var socket: Socket
    private var printWriter: PrintWriter? = null
    private var bufferedReader: BufferedReader ?= null

    private var thread: Thread
    private val running = AtomicBoolean(true)

    private val TAG = "VCamdroid"

    init {
        thread = Thread {
            try {
                socket = Socket(ipAddress, port)

                printWriter = PrintWriter(socket.getOutputStream())
                bufferedReader = BufferedReader(InputStreamReader(socket.getInputStream()))

                Log.d(TAG, "TCP Connected to $ipAddress:$port")

                send("AndroidTCPClient")
                startReceiveBytesLoop()
            } catch (e: Exception) {
                Log.e(TAG, "TCP Connection to $ipAddress:$port failed!" + e.message)
            }
        }

        thread.start()
    }

    fun send(data: String) {
        printWriter?.write(data)
        printWriter?.flush()
    }

    fun close() {
        running.set(false)
        thread.join()
        printWriter?.close()
        bufferedReader?.close()
        socket.close()
    }

    private fun startReceiveBytesLoop() {
        val buf = CharArray(15)
        while(running.get()) {
            try {
                val bytes = bufferedReader?.read(buf)
                listener.onBytesReceived(buf)
            } catch (e: SocketException) {
                Log.e(TAG, "Error while reading. Closing socket. ${e.message}")
                break
            }
        }
    }
}