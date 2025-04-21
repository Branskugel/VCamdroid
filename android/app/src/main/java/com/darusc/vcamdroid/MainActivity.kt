package com.darusc.vcamdroid

import android.graphics.Bitmap
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.util.Size
import androidx.camera.core.CameraExecutor
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.core.resolutionselector.ResolutionSelector
import androidx.camera.core.resolutionselector.ResolutionStrategy
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import com.darusc.vcamdroid.databinding.ActivityMainBinding
import com.darusc.vcamdroid.networking.TCPConnection
import com.darusc.vcamdroid.networking.UDPConnection
import java.nio.ByteBuffer
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {

    private lateinit var viewBinding: ActivityMainBinding

    private lateinit var cameraExecutor: ExecutorService
    private lateinit var udpConnection: UDPConnection

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        viewBinding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(viewBinding.root)

        udpConnection = UDPConnection("192.168.100.34", 6969)
        val tcpConnection = TCPConnection("192.168.100.34", 6969, object : TCPConnection.Listener {
            override fun onBytesReceived(bytes: CharArray, size: Int) {
                //

            }
        })

        startCamera()
        cameraExecutor = Executors.newSingleThreadExecutor()
    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)

        cameraProviderFuture.addListener({
            val resolutionSelector = ResolutionSelector.Builder()
                .setResolutionStrategy(ResolutionStrategy(Size(640, 480), ResolutionStrategy.FALLBACK_RULE_NONE))
                .build()

            val cameraProvider = cameraProviderFuture.get()
            val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA

            val preview = Preview.Builder().setResolutionSelector(resolutionSelector).build().also { it.surfaceProvider = viewBinding.viewFinder.surfaceProvider }

            val imageAnalyzer = ImageAnalysis.Builder().setResolutionSelector(resolutionSelector).setOutputImageFormat(ImageAnalysis.OUTPUT_IMAGE_FORMAT_RGBA_8888).build().also {
                it.setAnalyzer(cameraExecutor, object : ImageAnalysis.Analyzer {

                    private val byteArrayRGB = ByteArray(640 * 480 * 3)

                    override fun analyze(image: ImageProxy) {
                        if(image.width == 640 && image.height == 480) {
                            try {
                                val bitmap = image.toBitmap()

                                val byteBuffer = ByteBuffer.allocate(bitmap.byteCount)
                                val byteArray = ByteArray(bitmap.byteCount)

                                bitmap.copyPixelsToBuffer(byteBuffer)
                                byteBuffer.rewind()
                                byteBuffer.get(byteArray)

                                var j = 0
                                for (i in byteArrayRGB.indices step 3) {
                                    byteArrayRGB[i] = byteArray[j]
                                    byteArrayRGB[i + 1] = byteArray[j + 1]
                                    byteArrayRGB[i + 2] = byteArray[j + 2]
                                    j += 4
                                }

                                udpConnection.send(byteArrayRGB, byteArrayRGB.size)
                            } catch (e: Exception) {
                                println(e.message)
                            }
                        }
                        image.close()
                    }
                })
            }

            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(this, cameraSelector, preview, imageAnalyzer)
            } catch (e: Exception) {
                Log.e("VCamdroid", "Use case binding failed", e)
            }

        }, ContextCompat.getMainExecutor(this));
    }
}