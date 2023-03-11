package com.nrcan.http

object CryptoJNI {

    init {
        System.loadLibrary("cryptor")
    }

    private external fun encrypt(bytes: ByteArray): ByteArray?
    private external fun decrypt(bytes: ByteArray): String?

    fun Encrypt(str: String) = encrypt(str.toByteArray())

    fun Decrypt(bytes: ByteArray) = decrypt(bytes)

}