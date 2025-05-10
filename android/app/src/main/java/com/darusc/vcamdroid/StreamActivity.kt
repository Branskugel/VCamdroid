package com.darusc.vcamdroid

import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.util.Size
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.AspectRatio
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.constraintlayout.widget.ConstraintSet
import com.darusc.vcamdroid.databinding.ActivityStreamBinding
import com.darusc.vcamdroid.networking.ConnectionManager
import com.darusc.vcamdroid.video.Camera
import com.darusc.vcamdroid.video.toJpeg

class StreamActivity : AppCompatActivity(), ConnectionManager.ConnectionStateCallback {

    private val TAG = "VCamdroid"

    private lateinit var viewBinding: ActivityStreamBinding

    private lateinit var camera: Camera
    private var jpegQuality = 80
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

        camera.start()

        connectionManager.setOnBytesReceivedCallback { buffer, bytes ->
            val type = buffer[0]
            when(type) {
                ConnectionManager.PacketType.RESOLUTION -> {
                    val width = (buffer[1].toInt() and 0xFF) or (buffer[2].toInt() shl 8)
                    val height = (buffer[3].toInt() and 0xFF) or (buffer[4].toInt() shl 8)
                    camera.start(Size(width, height))
                    setPreviewAspectRatio(camera.aspectRation)
                }
                ConnectionManager.PacketType.CAMERA -> {
                    val back = buffer[1].toInt() == 0x01;
                    camera.start(
                        if(back) CameraSelector.DEFAULT_BACK_CAMERA
                        else CameraSelector.DEFAULT_FRONT_CAMERA
                    )
                }
                ConnectionManager.PacketType.QUALITY -> {
                    jpegQuality = buffer[1].toInt()
                }
                ConnectionManager.PacketType.WB -> {
                    camera.start(buffer[1].toInt())
                }
            }
        }
    }

    override fun onDisconnected() {
        Log.e(TAG, "Connection disconnected")
        Toast.makeText(this, "Connection interrupted!", Toast.LENGTH_LONG).show()
        finish()
    }

    private fun processImage(image: ImageProxy) {
        val bytes = image.toJpeg(jpegQuality)
        bytes?.let {
            connectionManager.sendVideoStreamFrame(bytes)
        }
        image.close()
    }

    private fun setPreviewAspectRatio(aspectRatio: String) {
        runOnUiThread {
            val constraintSet = ConstraintSet().apply {
                clone(viewBinding.root)
                setDimensionRatio(viewBinding.viewFinder.id, aspectRatio)
            }
            constraintSet.applyTo(viewBinding.root)
        }
    }
}