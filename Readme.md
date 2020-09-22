
# Web Keyboard

This is a very simple utility to send keyboard event to a server over http.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

# Usage

Suppose you have a Server at 192.168.0.1.

You have to copy the single executable to the Server, and start it as root (such
permissions are needed to create the virtual keyboard device file):

```
sudo ./webkeyboard.exe 8080
```

From any Client in the same sub-network, you can point your browser to 
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

The utility can be built simply with

```
gcc webkeyboard.c
```

however a build script is provided. It will download a full and self-contained
gcc toolchain from [musl.cc](https://musl.cc). If the `TARGET` environment
varible is set to `arm_linux`, a cross compilation toolchanchain will be used
and arm executables will be generated.

