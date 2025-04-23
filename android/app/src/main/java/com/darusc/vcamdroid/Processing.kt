package com.darusc.vcamdroid

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.camera.core.ImageProxy
import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer

fun ImageProxy.toRgbBytes(): ByteArray? {
    try {
        val bitmap = toBitmap()

        val byteBuffer = ByteBuffer.allocate(bitmap.byteCount)
        val byteArray = ByteArray(bitmap.byteCount)
        val byteArrayRGB = ByteArray(width * height * 3)

        // Extract the bytes from the bitmap bitmap
        bitmap.copyPixelsToBuffer(byteBuffer)
        byteBuffer.rewind()
        byteBuffer.get(byteArray)

        // Build the rgb byte array from the bitmap bytes
        // Note that the bitmap bytes are in RGBA format
        // so we need to discard every 4th byte as we don't
        // want the alpha channel
        var j = 0
        for (i in byteArrayRGB.indices step 3) {
            byteArrayRGB[i] = byteArray[j]
            byteArrayRGB[i + 1] = byteArray[j + 1]
            byteArrayRGB[i + 2] = byteArray[j + 2]
            j += 4
        }

        return byteArrayRGB
    } catch (e: Exception) {
        println(e.message)
    }
    return null
}

fun ImageProxy.toJpeg(quality: Int): ByteArray {
    val stream = ByteArrayOutputStream()
    val bitmap = toBitmap()
    bitmap.compress(Bitmap.CompressFormat.JPEG, quality, stream)
    return stream.toByteArray()
}