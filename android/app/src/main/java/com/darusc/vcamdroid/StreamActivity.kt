package com.darusc.vcamdroid

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
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
            ImageAnalysis.OUTPUT_IMAGE_FORMAT_RGBA_8888,
            ::processImage,
            this,
            this
        )
        camera.start()
    }

    override fun onDisconnected() {
        Log.e(TAG, "Connection disconnected")
    }

    private fun processImage(image: ImageProxy) {
        if (image.width == 640 && image.height == 480) {
            val bytes = image.toRgbBytes()
            image.close()

            bytes?.let {
                connectionManager.sendVideoStreamFrame(bytes)
            }
        }
        image.close()
    }
}