
# Web Keyboard

This is a very simple utility to send keyboard event to a server over http.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

# Installation

To install the last release, you can run the `util/webkey_update.sh` script on
the target machine. Altenatively you can run the following command:

```
curl -L -k https://raw.githubusercontent.com/pocomane/webkeyboard/master/util/webkey_mister.sh | bash -s update
```

in a shell on the target machine. Both the methods will download the last built
release and install it in the default path: `/media/fat/misc/webkeyboard`. It
will also generate start/stop script in the default path: `/media/fat/Scripts`.

# Usage

Suppose you have a Server at 192.168.0.1.

You can run the start script as root or run the following command in the
installation directory:

```
sudo ./webkeyboard.exe 8081
```

The root permissions are needed to create the virtual keyboard device file.

Now, from any Client in the same sub-network, you can point your browser to
http://192.168.0.1:8080.

While the browser window and tab have the focus, all the keyboard events of the
Client will be emulated on the server.

# Options

You can change the port to which webkeyboard is binded, passing it as the first
argument.  If the first argument is missing, webkeyboard will use a random
port. In any case it will print on the console the port it is bind to.

You can pass the port 80 as argument, so the client can connect directly to
https://192.168.0.1, but some system requires special configurations to let a
process bind to a port lower than 1024.

# Security

This application is deliberately very simple. It does have any security enforcement
(no TLS, no passwod, etc.). So please use it only in non-critical application.

# Building

Instead of installing the binary release, you can build the utility by yourself
with:

```
gcc webkeyboard.c
```

Altenatively a build script is provided. It will download a full and self-contained
gcc toolchain from [musl.cc](https://musl.cc). If the `TARGET` environment
varible is set to `arm_linux`, a cross compilation toolchanchain will be used
and arm executables will be generated.

