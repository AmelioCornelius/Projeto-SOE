#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define FROM    "tortadeframboesa744@gmail.com"
#define TO      "ludmyllacaetano1@gmail.com"
#define CC      "umadecisaohorrorosa@gmail.com"


static const char *payload_text[] = {
  "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n",
  "To: " TO "\r\n",
  "From: " FROM "Raspberry\r\n",
  "Cc: " CC "Rascunhos\r\n",
  "Subject: Relatório de temperaturas e uso de máscara facial\r\n",
  "\r\n", /* linha em branco */ 
  "Arquivo csv referente ao uso de máscara facial e temperaturas dos funcionários.\r\n",
  "\r\n",
  NULL
};
 
struct upload_status {
  int lines_read;
};
 
static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
 
  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }
 
  data = payload_text[upload_ctx->lines_read];
 
  if(data) {
    size_t len = strlen(data);
    memcpy(ptr, data, len);
    upload_ctx->lines_read++;
 
    return len;
  }
 
  return 0;
}

int main(void) {
  CURL *curl;
  CURLcode res = CURLE_OK;
  char errbuf[CURL_ERROR_SIZE];
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx;
 
  upload_ctx.lines_read = 0;

  curl = curl_easy_init();

  if(curl) {
    curl_mime *mime;
    curl_mimepart *part;
    /*URL do servidor de e-mail*/
    curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
    
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

    recipients = curl_slist_append(recipients, TO);
    recipients = curl_slist_append(recipients, CC);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_USERNAME, "tortadeframboesa744@gmail.com"); //e-mail
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "complexo58"); //senha

    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    //curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    // anexar arquivo
    mime = curl_mime_init(curl);
    part = curl_mime_addpart(mime);
    curl_mime_filedata(part, "dados.csv");
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_mime_encoder(part, "base64");

    /* Enviar a mensagem */
    res = curl_easy_perform(curl);
    /* Checar se há erros */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %d - %s\n%s\n",
              curl_easy_strerror(res));
              //res, curl_easy_strerror(res), errbuf);
        /* liberando */
    curl_slist_free_all(recipients);

    curl_easy_cleanup(curl);
    curl_mime_free(mime);
  }

  return (int)res;
}

/*void parseRecipients(char* emailAddrFileName, 
  struct curl_slist **recipientsPtr) {
    FILE *f = fopen(emailAddrFileName, "r");
    char emailAddress[65];

    if (f == NULL) {
        fprintf(stderr, "Erro abrindo o arquivo!\n");
        exit(1);
    }

    //carregando os endereços de e-mail
    for (int i = 0; ; i++) {
        if (fgets(emailAddress, 65, f) == NULL)
            break;
        emailAddress[strlen(emailAddress) - 1] = 0; //null
        if (strlen(emailAddress) == 0)
            break;
        *recipientsPtr = curl_slist_append(*recipientsPtr, emailAddress);
    }

    fclose(f);
}*/
