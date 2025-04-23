package com.darusc.vcamdroid

import android.content.Context
import android.media.Image
import android.util.Log
import android.util.Size
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageAnalysis.Analyzer
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.core.Preview.SurfaceProvider
import androidx.camera.core.resolutionselector.ResolutionSelector
import androidx.camera.core.resolutionselector.ResolutionStrategy
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner
import com.google.mlkit.vision.barcode.BarcodeScannerOptions
import com.google.mlkit.vision.barcode.BarcodeScanning
import com.google.mlkit.vision.barcode.common.Barcode
import com.google.mlkit.vision.common.InputImage
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/**
 * The ImageReadyListener is responsible for closing the ImageProxy
 */
typealias ImageReadyListener = (image: ImageProxy) -> Unit

class Camera(
    private val surface: Preview.SurfaceProvider,
    private val format: Int,
    private val imageReadyListener: ImageReadyListener,
    private val context: Context,
    private val lifecycleOwner: LifecycleOwner
) {

    private val TAG = "VCamdroid"
    private val cameraExecutor: ExecutorService = Executors.newSingleThreadExecutor()

    private fun buildResolutionSelector(resolution: Size) =
        ResolutionSelector.Builder()
            .setResolutionStrategy(ResolutionStrategy(resolution, ResolutionStrategy.FALLBACK_RULE_NONE))
            .build()

    private fun buildPreview(resolutionSelector: ResolutionSelector, surface: SurfaceProvider) =
        Preview.Builder()
            .setResolutionSelector(resolutionSelector)
            .build()
            .apply {
                surfaceProvider = surface
            }

    private fun buildAnalyzer(resolutionSelector: ResolutionSelector, analyzer: Analyzer) =
        ImageAnalysis.Builder()
            .setResolutionSelector(resolutionSelector)
            .setOutputImageFormat(format)
            .build()
            .apply {
                setAnalyzer(cameraExecutor, analyzer)
            }

    /**
     * Defaults to a resolution of 640x480 and to the back camera
     */
    fun start() {
        start(Size(640, 480), CameraSelector.DEFAULT_BACK_CAMERA)
    }

    fun start(resolution: Size, cameraSelector: CameraSelector) {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(context)

        cameraProviderFuture.addListener({

            val resolutionSelector = buildResolutionSelector(resolution)
            val cameraProvider = cameraProviderFuture.get()

            val preview = buildPreview(resolutionSelector, surface)

            val imageAnalyzer = buildAnalyzer(resolutionSelector) { image ->
                imageReadyListener(image)
            }

            try {
                cameraProvider.unbindAll()
                cameraProvider.bindToLifecycle(lifecycleOwner, cameraSelector, preview, imageAnalyzer)
            } catch (e: Exception) {
                Log.e("VCamdroid", "Use case binding failed", e)
            }

        }, ContextCompat.getMainExecutor(context));
    }
}