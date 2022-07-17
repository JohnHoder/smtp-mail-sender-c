/*Jan Hodermarsky, 2011*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>

#include "base64.h"

#define PORT 25 //465 //25 //587
#define FROM "xxx@gmail.com"
#define RECIPIENT "recip@gmail.com"
#define SMTP "smtp.xxx.cz"
#define TEXT "test 1234"

char login[] = "zzzz@gmail.com";
char password[] = "pass";

#define fatal(x...)	fprintf(stderr, x)
#define clean(x)	memset(x, '\0', sizeof(x))

void sendsock(u_int sock, u_char* format, ...);

int main(int argc, char *argv[]) {
	
    struct sockaddr_in home;
    struct hostent *hp;
    u_char buffer[2048], sockbuf[2048], tmp1[4096],  tmp2[4096];
    u_int sd, delay = 3;
    
    if (!(hp = gethostbyname(SMTP))) {
		fatal("Unknown host\n");
		return 1;
    }
    
    memset((u_char *)&home, 0, sizeof(home));
    home.sin_family = AF_INET;
    home.sin_port = htons(PORT);
    memcpy((u_char *)&home.sin_addr, hp->h_addr, hp->h_length);
        
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return 1;
    }
    
    if (connect(sd, (struct sockaddr *)&home, sizeof(home)) < 0) {
    	fatal("[!] Cant connect to host\n");
    	return -1;
    }

    clean(sockbuf);
    if (recv(sd, sockbuf, sizeof(sockbuf), 0) < 0)
    {
		fatal("recv() error\n");
		return 1;
    }
    
    if (sockbuf[0] != '2'){ /* if we didnt recieve MTA banner quit then */
		fatal("[!] Error on recieving the banner\n");
		return 1;
    }
    
    fprintf(stdout, "[+] Connected: %s", sockbuf);
    clean(sockbuf);

    //while woould start here
    //while (fgets(buffer, sizeof(buffer), rcpt)) {

	sendsock(sd, "EHLO boy\r\n");
	sendsock(sd, "AUTH LOGIN\r\n");

	encode(strlen(login), login, 128, tmp1); //Base64 encode login
	sendsock(sd, "%s\r\n", tmp1);
	encode(strlen(password), password, 128, tmp2); //Base64 encode login
	sendsock(sd, "%s\r\n", tmp2);

	sleep(delay);
	//recv(sd, sockbuf, sizeof(sockbuf), 0);
	//printf("%s\n", sockbuf);

	sendsock(sd, "MAIL FROM: <%s>\r\n", FROM);
	/*if (RECIPIENT[strlen(RECIPIENT)-1] == '\n')
	    RECIPIENT[strlen(RECIPIENT)-1] = '\0';*/
	sendsock(sd, "RCPT TO: <%s>\r\n", RECIPIENT);

	//sendsock(sd, "STARTTLS\r\n");

	recv(sd, sockbuf, sizeof(sockbuf), 0);
	printf("%s\n", sockbuf);
	if (sockbuf[0] == '5') {
	    fatal("[!] Someting goes wrong: %s\n", sockbuf);
	    return 1;
	}
	clean(sockbuf);
	sendsock(sd, "DATA\r\n");
	/* Is relaying allowed here ? */
	recv(sd, sockbuf, sizeof(sockbuf), 0);
	if (sockbuf[0] == '5') {
	    fatal("[!] Something goes wrong: %s", sockbuf);
	    return 1;
	}
	clean(sockbuf);
	
	sleep(delay);

	/* dump file into socket */
	/*spam = fopen("file.txt", "r");
	while (fgets(tmp1, sizeof(tmp1), spam))
	    sendsock(sd, tmp1);*/

	sendsock(sd, "%s\r\n", TEXT);
	sendsock(sd, ".\r\n");
	sleep(delay);

	recv(sd, sockbuf, sizeof(sockbuf), 0);
	printf("%s\n", sockbuf);

	//fprintf(stdout, "FINISH");
	clean(RECIPIENT);

	//end while    
    
    sleep(delay);
    sendsock(sd, "QUIT\r\n");
    return 0;
}

void sendsock(u_int sock, u_char* format, ...) {
    char buf[4096];
    va_list args;
    va_start(args, format);
    clean(buf);
    vsnprintf(buf, sizeof(buf), format, args);
    write(sock, buf, strlen(buf));
    va_end(args);
}
