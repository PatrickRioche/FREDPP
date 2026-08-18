package fr.fredpp.android

object NativeBridge {
    init {
        System.loadLibrary("fredpp_android")
    }

    external fun createSession(): Long
    external fun destroySession(handle: Long)
    external fun executeLine(handle: Long, source: String): String
    external fun prompt(handle: Long): String
    external fun version(): String
}

class NativeSession : AutoCloseable {
    private var handle: Long = NativeBridge.createSession()

    val isAvailable: Boolean
        get() = handle != 0L

    fun executeLine(source: String): String {
        if (handle == 0L) return "error: moteur FREDPP indisponible\n"
        return NativeBridge.executeLine(handle, source)
    }

    fun prompt(): String {
        if (handle == 0L) return "?> "
        return NativeBridge.prompt(handle)
    }

    fun reset() {
        close()
        handle = NativeBridge.createSession()
    }

    override fun close() {
        if (handle != 0L) {
            NativeBridge.destroySession(handle)
            handle = 0L
        }
    }
}
