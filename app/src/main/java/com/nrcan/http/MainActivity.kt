package com.nrcan.http

import android.os.Build
import android.os.Bundle
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.nrcan.http.databinding.ActivityMainBinding
import kotlinx.coroutines.DelicateCoroutinesApi


class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    @RequiresApi(Build.VERSION_CODES.O)
    @OptIn(DelicateCoroutinesApi::class)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()


        val needEncryptValue = "iwejfioceomocmowdmocmodwmcoqjdioejjdiweojfmweo"

    }


    /**
     * A native method that is implemented by the 'http' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'http' library on application startup.
        init {
            System.loadLibrary("http")
        }
    }
}