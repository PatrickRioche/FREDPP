package fr.fredpp.android

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent { FredppApp() }
    }
}

@Composable
private fun FredppApp() {
    val session = remember { NativeSession() }
    val scrollState = rememberScrollState()
    val focusRequester = remember { FocusRequester() }

    var input by remember { mutableStateOf("") }
    var prompt by remember { mutableStateOf(session.prompt()) }
    var transcript by remember {
        mutableStateOf(
            "FREDPP v${runCatching { NativeBridge.version() }.getOrDefault("0.0.22")}\n" +
                "Type ? for FRED help; type ?: for FREDPP commands; type Q or QQ to exit.\n"
        )
    }

    fun submitLine() {
        val line = input

        if (line == ":cls") {
            transcript = ""
            input = ""
            prompt = session.prompt()
            return
        }

        transcript += "$prompt$line\n"

        val result = session.executeLine(line)
        if (result.isNotEmpty()) {
            transcript += result
            if (!result.endsWith("\n")) {
                transcript += "\n"
            }
        }

        input = ""
        prompt = session.prompt()
    }

    DisposableEffect(Unit) {
        onDispose { session.close() }
    }

    LaunchedEffect(Unit) {
        focusRequester.requestFocus()
    }

    LaunchedEffect(transcript) {
        scrollState.scrollTo(scrollState.maxValue)
    }

    MaterialTheme {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .background(Color(0xFF080D18))
                .imePadding()
                .padding(
                    start = 10.dp,
                    end = 10.dp,
                    top = 8.dp,
                    bottom = 6.dp
                )
        ) {
            Row(
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Image(
                    painter = painterResource(R.drawable.fredpp_logo),
                    contentDescription = "Logo officiel FREDPP",
                    modifier = Modifier.size(50.dp)
                )

                Spacer(Modifier.width(8.dp))

                Column(modifier = Modifier.weight(1f)) {
                    Text(
                        "FREDPP Android",
                        color = Color(0xFFD6B45F),
                        style = MaterialTheme.typography.titleSmall
                    )
                    Text(
                        "moteur C++20 natif",
                        color = Color(0xFFAAB7CF),
                        style = MaterialTheme.typography.bodySmall
                    )
                }

                OutlinedButton(
                    onClick = {
                        session.reset()
                        input = ""
                        prompt = session.prompt()
                        transcript =
                            "FREDPP v${NativeBridge.version()}\n" +
                                "[session réinitialisée]\n"
                    }
                ) {
                    Text("Reset")
                }
            }

            Spacer(Modifier.height(6.dp))

            /*
             * Terminal :
             * - lecture seule ;
             * - occupe tout l'espace restant ;
             * - défile verticalement au doigt.
             */
            Surface(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1f),
                color = Color(0xFF101827)
            ) {
                Text(
                    text = transcript,
                    color = Color(0xFFDDE7FF),
                    fontFamily = FontFamily.Monospace,
                    style = MaterialTheme.typography.bodyMedium,
                    modifier = Modifier
                        .fillMaxSize()
                        .verticalScroll(scrollState)
                        .padding(10.dp)
                )
            }

            Spacer(Modifier.height(6.dp))

            /*
             * Ligne de commande fixe.
             * imePadding() sur le conteneur principal la maintient
             * au-dessus du clavier Android.
             */
            Row(
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.spacedBy(6.dp)
            ) {
                Text(
                    text = prompt,
                    color = Color(0xFFD6B45F),
                    fontFamily = FontFamily.Monospace
                )

                OutlinedTextField(
                    value = input,
                    onValueChange = { input = it },
                    enabled = session.isAvailable,
                    singleLine = true,
                    modifier = Modifier
                        .weight(1f)
                        .focusRequester(focusRequester),
                    textStyle = MaterialTheme.typography.bodyMedium.copy(
                        fontFamily = FontFamily.Monospace,
                        color = Color.White
                    ),
                    keyboardOptions = KeyboardOptions(
                        imeAction = ImeAction.Send
                    ),
                    keyboardActions = KeyboardActions(
                        onSend = { submitLine() }
                    )
                )
            }
        }
    }
}
