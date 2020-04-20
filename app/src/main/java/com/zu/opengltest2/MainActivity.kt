package com.zu.opengltest2

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.ViewGroup
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {


    private lateinit var surfaceView: SurfaceView
    private var surfaceViewCallback: SurfaceHolder.Callback = object : SurfaceHolder.Callback{
        override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
            nSetSize(width, height)
        }

        override fun surfaceDestroyed(holder: SurfaceHolder?) {
//            nReleaseSurface()
        }

        override fun surfaceCreated(holder: SurfaceHolder?) {
            nInit()
            nStart()
            nSetSurface(holder!!.surface)


        }
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        addSurfaceView()
    }

    private fun addSurfaceView()
    {
        surfaceView = SurfaceView(this)
        var holder = surfaceView.holder
        holder.addCallback(surfaceViewCallback)
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS)
        var layoutParams = ViewGroup.LayoutParams(1920, 1080)
        layoutParams.width = ViewGroup.LayoutParams.MATCH_PARENT
        layoutParams.height = ViewGroup.LayoutParams.MATCH_PARENT
        surfaceView.layoutParams = layoutParams
        root_layout.addView(surfaceView)
    }

    override fun onDestroy() {
        nStop()
        nDestroy()
        super.onDestroy()
    }

    external fun nInit()
    external fun nDestroy()
    external fun nStart()
    external fun nStop()
    external fun nSetSurface(surface: Any)
    external fun nReleaseSurface()
    external fun nSetSize(width: Int, height: Int)
    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }
}
