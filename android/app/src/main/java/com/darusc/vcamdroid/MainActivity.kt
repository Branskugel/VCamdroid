package com.darusc.vcamdroid

import android.content.Context
import android.content.IntentFilter
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.util.Size
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.core.resolutionselector.ResolutionSelector
import androidx.camera.core.resolutionselector.ResolutionStrategy
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import com.darusc.vcamdroid.databinding.ActivityMainBinding
import com.darusc.vcamdroid.networking.ConnectionManager
import java.nio.ByteBuffer
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity(), ConnectionManager.ConnectionStateCallback {

    private lateinit var viewBinding: ActivityMainBinding

    private lateinit var cameraExecutor: ExecutorService
    private val connectionManager = ConnectionManager(this)

    private val TAG = "VCamdroid"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        viewBinding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(viewBinding.root)

        // Prioritize the usb connection through adb
        if(hasUsbConnection()) {
            connectionManager.connect(6969)
        } else if(hasWifiConnection()) {
            connectionManager.connect("192.168.100.34", 6969)
        }

        //startCamera()
        cameraExecutor = Executors.newSingleThreadExecutor()
    }

    override fun onConnectionSuccessful(connectionMode: ConnectionManager.Mode) {
        Log.d(TAG, "Connection successful $connectionMode")
        startCamera()
    }

    override fun onConnectionFailed(connectionMode: ConnectionManager.Mode) {
        if(connectionMode == ConnectionManager.Mode.USB && hasWifiConnection()) {
            // If usb connection failed try again over wifi
            connectionManager.connect("192.168.100.34", 6969)
        } else {
            Log.e(TAG, "Error: Cannot connect!")
        }
    }

    override fun onDisconnected() {
        Log.d(TAG, "Connection disconnected")
    }

    private fun hasUsbConnection(): Boolean {
        val intent = applicationContext.registerReceiver(null, IntentFilter("android.hardware.usb.action.USB_STATE"))
        return intent?.getBooleanExtra("connected", false) == true
    }

    private fun hasWifiConnection(): Boolean {
        val connectivityManager = applicationContext.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        val network = connectivityManager.activeNetwork ?: return false
        val capabilities = connectivityManager.getNetworkCapabilities(network) ?: return false
        return capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)
    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)

        cameraProviderFuture.addListener({
            val resolutionSelector = ResolutionSelector.Builder()
                .setResolutionStrategy(ResolutionStrategy(Size(640, 480), ResolutionStrategy.FALLBACK_RULE_NONE))
                .build()

            val cameraProvider = cameraProviderFuture.get()
            val cameraSelector = CameraSelector.DEFAULT_BACK_CAMERA

            val preview = Preview.Builder()
                .setResolutionSelector(resolutionSelector)
                .build()
                .apply {
                    surfaceProvider = viewBinding.viewFinder.surfaceProvider
                }

            val imageAnalyzer = ImageAnalysis.Builder()
                .setResolutionSelector(resolutionSelector)
                .setOutputImageFormat(ImageAnalysis.OUTPUT_IMAGE_FORMAT_RGBA_8888)
                .build()
                .apply {
                    setAnalyzer(cameraExecutor, object : ImageAnalysis.Analyzer {
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

                                    connectionManager.sendVideoStreamFrame(byteArrayRGB)
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