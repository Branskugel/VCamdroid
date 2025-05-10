package com.darusc.vcamdroid

import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Rect
import android.hardware.camera2.CaptureRequest
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.util.Size
import android.view.View
import android.view.WindowManager
import androidx.activity.enableEdgeToEdge
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.view.PreviewView
import com.darusc.vcamdroid.databinding.ActivityMainBinding
import com.darusc.vcamdroid.networking.ConnectionManager
import com.darusc.vcamdroid.video.Camera

class MainActivity : AppCompatActivity(), ConnectionManager.ConnectionStateCallback {

    private lateinit var viewBinding: ActivityMainBinding

    private val qrscanner = QRScanner()
    private var connectionManager = ConnectionManager.getInstance(this)
    private lateinit var camera: Camera

    private var isConnecting = false

    private val TAG = "VCamdroid"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        viewBinding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(viewBinding.root)

        enableEdgeToEdge()

        camera = Camera(
            viewBinding.viewFinder.surfaceProvider,
            ImageAnalysis.OUTPUT_IMAGE_FORMAT_YUV_420_888,
            ::processImage,
            this,
            this
        )
        camera.start(Size(1280, 720), CameraSelector.DEFAULT_BACK_CAMERA, CaptureRequest.CONTROL_AWB_MODE_AUTO, CaptureRequest.CONTROL_EFFECT_MODE_OFF)

        // Prioritize the usb connection through adb
        if (hasUsbConnection()) {
            connectUSB()
        } else if (hasWifiConnection()) {
            connectWIFI()
        }
    }

    override fun onResume() {
        super.onResume()
        connectionManager = ConnectionManager.getInstance(this)
        camera.start(Size(1280, 720), CameraSelector.DEFAULT_BACK_CAMERA, CaptureRequest.CONTROL_AWB_MODE_AUTO, CaptureRequest.CONTROL_EFFECT_MODE_OFF)
        connectWIFI()
    }

    override fun onConnectionSuccessful(connectionMode: ConnectionManager.Mode) {
        qrscanner.stop()
        Log.d(TAG, "Connection successful $connectionMode")

        val intent = Intent(this, StreamActivity::class.java)
        startActivity(intent)
    }

    override fun onConnectionFailed(connectionMode: ConnectionManager.Mode) {
        if (connectionMode == ConnectionManager.Mode.USB && hasWifiConnection()) {
            // If usb connection failed try again over wifi
            connectWIFI()
        } else {
            isConnecting = false
            qrscanner.stop()
            Log.e(TAG, "Error: Cannot connect!")
        }
    }

    private fun processImage(imageProxy: ImageProxy) {
        // Process each incoming image from the camera
        // launchScanTask() will scan and call the callback on success
        // only if start() was called before
        qrscanner.launchScanTask(imageProxy, camera.screenRectToImageRect(viewBinding.overlay.rect, viewBinding.overlay.size)) { result ->
            if(result != null) {
                connectionManager.connect(result.address, result.port)
            } else {
                Log.d(TAG, "Invalid QR code")
            }
        }
    }

    private fun hasUsbConnection(): Boolean {
        val intent = applicationContext.registerReceiver(
            null,
            IntentFilter("android.hardware.usb.action.USB_STATE")
        )
        return intent?.getBooleanExtra("connected", false) == true
    }

    private fun hasWifiConnection(): Boolean {
        val connectivityManager = applicationContext.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        val network = connectivityManager.activeNetwork ?: return false
        val capabilities = connectivityManager.getNetworkCapabilities(network) ?: return false
        return capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)
    }

    private fun connectUSB() {
        connectionManager.connect(6969)
    }

    private fun connectWIFI() {
        // Start the QRScanner so it can scan the image frames received from the camera
        // Actual WIFI connection is tried only on scan success
        qrscanner.start()
    }
}