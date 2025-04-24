package com.darusc.vcamdroid

import android.os.Bundle
import android.util.Log
import android.util.Size
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import com.darusc.vcamdroid.databinding.ActivityStreamBinding
import com.darusc.vcamdroid.networking.Connection
import com.darusc.vcamdroid.networking.ConnectionManager

class StreamActivity : AppCompatActivity(), ConnectionManager.ConnectionStateCallback {

    private val TAG = "VCamdroid"

    private lateinit var viewBinding: ActivityStreamBinding

    private lateinit var camera: Camera
    private val connectionManager: ConnectionManager = ConnectionManager.getInstance(this)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        viewBinding = ActivityStreamBinding.inflate(layoutInflater)
        setContentView(viewBinding.root)

        camera = Camera(
            viewBinding.viewFinder.surfaceProvider,
            ImageAnalysis.OUTPUT_IMAGE_FORMAT_YUV_420_888,
            ::processImage,
            this,
            this
        )

        startCamera(640, 480, CameraSelector.DEFAULT_BACK_CAMERA)

        connectionManager.setOnBytesReceivedCallback { buffer, bytes ->
            val type = buffer[0]
            when(type) {
                ConnectionManager.PacketType.RESOLUTION -> {
                    val width = (buffer[1].toInt() and 0xFF) or (buffer[2].toInt() shl 8)
                    val height = (buffer[3].toInt() and 0xFF) or (buffer[4].toInt() shl 8)
                    startCamera(width, height, CameraSelector.DEFAULT_BACK_CAMERA)
                }
            }
        }
    }

    override fun onDisconnected() {
        Log.e(TAG, "Connection disconnected")
    }

    private fun startCamera(width: Int, height: Int, camera: CameraSelector) {
        this.camera.start(Size(width, height), camera)
    }

    private fun processImage(image: ImageProxy) {
        val bytes = image.toJpeg(80)
        image.close()

        bytes?.let {
            connectionManager.sendVideoStreamFrame(bytes)
        }
        image.close()
    }
}