
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

This application was developed mainly to operate on a
[MiSTer](https://github.com/MiSTer-devel/Main_MiSTer/wiki). For that platform there
is a simple scripts to install/update it; you can find it at
[MiSTer_misc](https://github.com/pocomane/MiSTer_misc).

For other linux on x86 or ARMs (with hard float), there are some package in the
[Release page](https://github.com/pocomane/webkeyboard/releases/latest): you just
need to download and extract one of them.

In the case you want to build it from source, if you have a working C toolchain, it
should be as simple as running:

```
gcc -static -o webkeyboard.exe webkeyboard.c
```

Altenatively a build script is provided. It will download a full and self-contained
gcc toolchain from [musl.cc](https://musl.cc). If the `TARGET` environment
varible is set to `arm_linux`, a cross compilation toolchanchain will be used
and arm executables will be generated.


# Usage

Suppose you have a Server at 192.168.0.1, you have to run the binary with root
privilages, i.e. if the binary is in the current directory:

```
sudo ./webkeyboard.exe 8081
```

The root permissions are needed to create the virtual keyboard device file.

Now, from any Client in the same sub-network, you can point your browser to
http://192.168.0.1:8081.

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

