
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ININFO(...)do{; fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }while(0)
#define ERROR(...) do{; fprintf(stderr, "ERROR "); ININFO(__VA_ARGS__); }while(0)
#define FATAL(...) do{ ERROR(__VA_ARGS__); exit(-1); }while(0)
#define LOG(...)   do{ fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); }while(0)
// #define DEBUG(...) do{ fprintf(stderr, "DEBUG %s:%d ", __FILE__, __LINE__); ININFO(__VA_ARGS__); }while(0)
#define DEBUG(...) (0)
#define LOG_UPDATE() do{ fflush(stderr); fflush(stdout); }while(0)

#define ARRSIZ(A) (sizeof(A)/sizeof(*A))
#define ARRASIZ(A) (A),(ARRSIZ(A))
#define STRASIZ(A) (A),(ARRSIZ(A)-1)

#define HTTPVER "HTTP/1.1 "
#define SERVEROK HTTPVER"200 OK\r\nServer: simple_server\r\nContent-Type: text/plain\r\nConnection: close\r\n"
#define SERVERERR HTTPVER"404 Not Found\r\nServer: simple_server\r\nContent-Type: text/plain\r\nConnection: close\r\n"

#define MAXWEBARG 9
extern const char *APP;
extern const int APPSIZ;

#include <fcntl.h>
#include <linux/uinput.h>
#define DEVICE_NAME "Fake device"
#define EV_DO_NOTHING EV_MAX+1
#define EV_EXTRA_CLOSE EV_MAX+2
static void ev_emit(int type, int code, int val){
  static int fd = -1;

  if (fd <= 0) {
    // Open the output device file
    
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
      FATAL("Can not open the output device file");
      return;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    for (int i=KEY_ESC; i<KEY_MAX; i++){
      ioctl(fd, UI_SET_KEYBIT, i);
    }

#if UINPUT_VERSION < 5
    struct uinput_user_dev uud;
    memset(&uud, 0, sizeof(uud));
    snprintf(uud.name, UINPUT_MAX_NAME_SIZE, DEVICE_NAME);
    write(fd, &uud, sizeof(uud));
#ifdef MISTER_FPGA_CHECK
#error MiSTer needs UINPUT 5 or above
#endif
#else
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234; // fake vendor
    usetup.id.product = 0x5678; // fake product
    strcpy(usetup.name, DEVICE_NAME);
    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
#endif
  }

  if (fd > 0) {
    if (type == EV_EXTRA_CLOSE) {
      // Close the output device file
      
      ioctl(fd, UI_DEV_DESTROY);
      close(fd);
      fd = -1;

    } else {
      // Emulate the key event

      if (type==EV_KEY) {
        DEBUG("emulating %x key %s",code,val?"press":"release");
      } else if (type==EV_SYN) {
        DEBUG("flushing event");
      }
      struct input_event ie = {0,};
      ie.type = type;
      ie.code = code;
      ie.value = val;
      // gettimeofday(&ie.time, NULL);
      write(fd, &ie, sizeof(ie));
    }
  }
}

static int cmp_char_ptr_field(const void * a, const void * b) {
  return strcmp(*(const char**)a, *(const char**)b);
}
#define SBSEARCH(T, SA, C)	(bsearch(T, SA, sizeof(SA)/sizeof(SA[0]), sizeof(SA[0]), (C)))
int js_to_linux_key(const char* wanted){
  struct map {
    const char * key;
    int value;
  } cmdlist[] = {
    //
    // The "key" field can not contain ' ' or '\0'.
    // The array must be lexicographically sorted wrt "key" field, e.g. place one
    // one entry per line and use the :sort vim command (but please mind '!' and
    // escaped chars at end of similar names).
    //
    {"AltLeft" , KEY_LEFTALT , } ,
    {"AltRight" , KEY_RIGHTALT , } ,
    {"ArrowDown" , KEY_DOWN , } ,
    {"ArrowLeft" , KEY_LEFT , } ,
    {"ArrowRight" , KEY_RIGHT , } ,
    {"ArrowUp" , KEY_UP , } ,
    {"Backquote" , KEY_GRAVE , } ,
    {"Backslash" , KEY_BACKSLASH , } ,
    {"Backsspace" , KEY_BACKSPACE , } ,
    {"BracketLeft" , KEY_LEFTBRACE , } ,
    {"BracketRight" , KEY_RIGHTBRACE , } ,
    {"Comma" , KEY_COMMA , } ,
    {"ControlLeft" , KEY_LEFTCTRL , } ,
    {"ControlRight" , KEY_RIGHTCTRL , } ,
    {"Delete" , KEY_DELETE , } ,
    {"Digit0" , KEY_0 , } ,
    {"Digit1" , KEY_1 , } ,
    {"Digit2" , KEY_2 , } ,
    {"Digit3" , KEY_3 , } ,
    {"Digit4" , KEY_4 , } ,
    {"Digit5" , KEY_5 , } ,
    {"Digit6" , KEY_6 , } ,
    {"Digit7" , KEY_7 , } ,
    {"Digit8" , KEY_8 , } ,
    {"Digit9" , KEY_9 , } ,
    {"End" , KEY_END , } ,
    {"Enter" , KEY_ENTER , } ,
    {"Equal" , KEY_EQUAL , } ,
    {"Escape" , KEY_ESC , } ,
    {"F1" , KEY_F1 , } ,
    {"F10" , KEY_F10 , } ,
    {"F11" , KEY_F11 , } ,
    {"F12" , KEY_F12 , } ,
    {"F2" , KEY_F2 , } ,
    {"F3" , KEY_F3 , } ,
    {"F4" , KEY_F4 , } ,
    {"F5" , KEY_F5 , } ,
    {"F6" , KEY_F6 , } ,
    {"F7" , KEY_F7 , } ,
    {"F8" , KEY_F8 , } ,
    {"F9" , KEY_F9 , } ,
    {"Home" , KEY_HOME , } ,
    {"Insert" , KEY_INSERT , } ,
    {"KeyA" , KEY_A , } ,
    {"KeyB" , KEY_B , } ,
    {"KeyC" , KEY_C , } ,
    {"KeyD" , KEY_D , } ,
    {"KeyE" , KEY_E , } ,
    {"KeyF" , KEY_F , } ,
    {"KeyG" , KEY_G , } ,
    {"KeyH" , KEY_H , } ,
    {"KeyI" , KEY_I , } ,
    {"KeyJ" , KEY_J , } ,
    {"KeyK" , KEY_K , } ,
    {"KeyL" , KEY_L , } ,
    {"KeyM" , KEY_M , } ,
    {"KeyN" , KEY_N , } ,
    {"KeyO" , KEY_O , } ,
    {"KeyP" , KEY_P , } ,
    {"KeyQ" , KEY_Q , } ,
    {"KeyR" , KEY_R , } ,
    {"KeyS" , KEY_S , } ,
    {"KeyT" , KEY_T , } ,
    {"KeyU" , KEY_U , } ,
    {"KeyV" , KEY_V , } ,
    {"KeyW" , KEY_W , } ,
    {"KeyX" , KEY_X , } ,
    {"KeyY" , KEY_Y , } ,
    {"KeyZ" , KEY_Z , } ,
    {"Minus" , KEY_MINUS , } ,
    {"OSLeft" , KEY_LEFTMETA , } ,
    {"PageDown" , KEY_PAGEDOWN , } ,
    {"PageUp" , KEY_PAGEUP , } ,
    {"Period" , KEY_DOT , } ,
    {"Quote" , KEY_APOSTROPHE , } ,
    {"Semicolon" , KEY_SEMICOLON , } ,
    {"ShiftLeft" , KEY_LEFTSHIFT , } ,
    {"ShiftRight" , KEY_RIGHTSHIFT , } ,
    {"Slash" , KEY_SLASH , } ,
    {"Space" , KEY_SPACE , } ,
    {"Tab" , KEY_TAB , } ,
  };
  struct map target = { wanted, };
  struct map *command = (struct map*) SBSEARCH(&target, cmdlist, cmp_char_ptr_field);
  if (NULL == command) return -1;
  return command->value;
}

int execute_command(int argc, char** argv){
  if (argc != 2) return -1;
  int code = js_to_linux_key(argv[1]);
  if (code < 0) return -1;
  if (0){
  } else if (!strcmp("press",argv[0])) {
    ev_emit(EV_KEY,code,1);
    ev_emit(EV_SYN,SYN_REPORT,0);
    return 0;
  } else if (!strcmp("release",argv[0])) {
    ev_emit(EV_KEY,code,0);
    ev_emit(EV_SYN,SYN_REPORT,0);
    return 0;
  }
  return 0;
}

int parse_and_execute_command(char * commandline){
  char*argv[MAXWEBARG] = {0,};
  char *cur = strchr(commandline,'/');
  if (!cur) return -1;
  *cur='\0';
  cur++;
  int argc = 0;
  for (int a =0; a<MAXWEBARG && *cur != '\0'; a+=1){
    argc+=1;
    argv[a]=cur;
    char *next = strchr(cur,'/');
    if (!next) break;
    *next='\0';
    cur=next+1;
  }
  DEBUG("running command");
  for (int a = 0; a < argc; a += 1)
    DEBUG("arg %d [%s]", a, argv[a]);
  return execute_command(argc, argv);
}

void write_not_found(int out){
  if (0> write(out,STRASIZ(SERVERERR"\r\n")))
    ERROR("writing to socket");
}

void execute_request(char * buffer, int out){
  char* begin = strchr(buffer,' ');
  if (0 == begin) return;
  begin++;
  char* end = strchr(begin,' ');
  if (0 == end) return;
  *end = '\0';
  if (!strcmp("/favicon.ico",begin)) {
    write_not_found(out); // No icon
  }else if (strcmp("/",begin)) {
    int err = parse_and_execute_command(buffer);
    if (err) {
      write_not_found(out);
    }else{
      int n = write(out,STRASIZ(SERVERERR"\r\n"));
      if (n < 0) ERROR("writing to socket");
    }
  } else {
    int n;
    n = write(out,STRASIZ(SERVEROK"Content-Type: text/html\r\n\r\n"));
    if (n < 0) ERROR("writing to socket");
    n = write(out,APP,APPSIZ);
    if (n < 0) ERROR("writing to socket");
    n = write(out,STRASIZ("\r\n\r\n"));
    if (n < 0) ERROR("writing to socket");
  }
}

int web_server_loop(int port, void (*handler)(char * buffer, int out)) {
 
  // Open the TCP socket and bind it to the wanted port
  int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket < 0) 
    FATAL("opening socket");
  int set_true = 1;
  struct sockaddr_in local_address;
  setsockopt(listen_socket,SOL_SOCKET,SO_REUSEADDR,&set_true,sizeof(int));
  bzero((char *) &local_address, sizeof(local_address));
  socklen_t address_len;
  local_address.sin_family = AF_INET;
  local_address.sin_addr.s_addr = INADDR_ANY;
  local_address.sin_port = htons(port);
  if (0> bind(listen_socket, (struct sockaddr *) &local_address, sizeof(local_address))) 
    FATAL("on binding");

  // Print the actual binded port 
  struct sockaddr local_info;
  bzero(&local_info, sizeof(local_info));
  address_len = sizeof(local_info);
  if (0 != getsockname(listen_socket, &local_info, &address_len))
    FATAL("opening socket");
  struct sockaddr_in *sin = (struct sockaddr_in *)&local_info;
  LOG("Listening on port %d",htons(sin->sin_port));

  // Enter Server mode
  listen(listen_socket,5);

  // Infinte loop for key-press handling
  while (1) {
    LOG_UPDATE();

    // Wait request
    struct sockaddr_in remote_address;
    socklen_t clilen = sizeof(remote_address);
    int connection_socket = accept(listen_socket, 
                (struct sockaddr *) &remote_address, 
                &clilen);
    if (connection_socket < 0) 
        FATAL("on accept");

    // Read request
    char buffer[256];
    bzero(ARRASIZ(buffer));
    int n = read(connection_socket,ARRASIZ(buffer));
    if (n < 0)
      FATAL("reading from socket");
    char* eol = strchr(buffer,'\n');
    if (!eol) {
      DEBUG("Got single line message: %s",buffer);
    } else {
      int len = eol - buffer - 1;
      DEBUG("Got message: %*.*s",len,len,buffer);
    }

    (*handler)(buffer,connection_socket);

    // Close connection with the client
    shutdown(connection_socket,SHUT_WR);
    close(connection_socket);
  }
  // Never reached
  close(listen_socket);
  return 0; 
}

int main(int argc, char *argv[]) {
 
  // Parsse command line argument 
  int port = 0;
  if (argc > 1)
    port = atoi(argv[1]);

  // Bring up the virtual device
  ev_emit(EV_DO_NOTHING,0,0);

  // Run the app loop
  return web_server_loop(port, &execute_request);
}

// Client-side application
char APP_CONTENT[] = ""
  "<html>"
    "<head>"
    "</head>"
    "<body>"
      "<script>"
        
        "function key_send(code, val){"
          "var api = 'release';"
          "if (val != 0) api = 'press';"
          "var xhr = new XMLHttpRequest();"
          "xhr.open('GET', '/'+api+'/'+code.toString(16), true);"
          "xhr.send(null);"
        "}"
        
        "document.addEventListener('keydown', function(ev){"
          "ev.preventDefault();"
          "if (ev.repeat) { return; };"
          "key_send(ev.code, 1);"
        "});"
        
        "document.addEventListener('keyup', function(ev){"
          "ev.preventDefault();"
          "key_send(ev.code, 0);"
        "});"

      "</script>"

      "While this page have focus, all the keyboard events will be sent to the server."
    "</body>"
  "</html>"
;
const char *APP = APP_CONTENT;
const int APPSIZ = ARRSIZ(APP_CONTENT)-1;

