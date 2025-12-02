/****************************************************************************
 * apps/examples/espcli/espcli_main.c
 *
 * Uso:
 *   espcli <IP> <mensaje...> [--no-wait|-n]
 *
 * Descripcion:
 *   Envia con el siguiente formato:
 *     "IP MENSAJE\n"
 *
 * Opciones:
 *   --no-wait, -n   (se acepta pero hoy en dia siempre es no-wait)
 *   --help, -h      Muestra esta ayuda.
 *
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>

#define ESPCLI_TTY_PATH   "/dev/ttyS1"
#define ESPCLI_TX_BUFSZ   256

/****************************************************************************
 * Name: espcli_usage
 ****************************************************************************/

static void espcli_usage(const char *progname)
{
  printf("Uso:\n");
  printf("  %s <IP> <mensaje...> [--no-wait|-n]\n\n", progname);

  printf("Descripcion:\n");
  printf("  Envia con el siguiente formato:\n");
  printf("    \"IP MENSAJE\\n\"\n\n");

  printf("Opciones:\n");
  printf("  --no-wait, -n   Envia el mensaje y termina sin esperar respuesta.\n");
  printf("  --help, -h      Muestra esta ayuda.\n\n");
}

/****************************************************************************
 * Name: espcli_main
 ****************************************************************************/

int espcli_main(int argc, char *argv[])
{
  int fd = -1;
  struct termios tio;
  char txbuf[ESPCLI_TX_BUFSZ];
  size_t i;
  int ret = EXIT_FAILURE;

  /* Solicitud de ayuda */

  if (argc >= 2)
    {
      if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
          espcli_usage(argv[0]);
          return EXIT_SUCCESS;
        }
    }

  /* Ignoramos realmente --no-wait/-n, pero lo aceptamos para no romper scripts */

  if (argc >= 3)
    {
      const char *last = argv[argc - 1];

      if (strcmp(last, "--no-wait") == 0 || strcmp(last, "-n") == 0)
        {
          argc--; /* lo quitamos de la lista, nada mas */
        }
    }

  /* Validar argumentos */

  if (argc < 3)
    {
      fprintf(stderr, "Error: argumentos insuficientes.\n\n");
      espcli_usage(argv[0]);
      return EXIT_FAILURE;
    }

  /* Construir la línea "IP MENSAJE\n" */

  memset(txbuf, 0, sizeof(txbuf));

  strlcpy(txbuf, argv[1], sizeof(txbuf));
  strlcat(txbuf, " ", sizeof(txbuf));

  for (i = 2; i < (size_t)argc; i++)
    {
      strlcat(txbuf, argv[i], sizeof(txbuf));
      if (i + 1 < (size_t)argc)
        {
          strlcat(txbuf, " ", sizeof(txbuf));
        }
    }

  strlcat(txbuf, "\n", sizeof(txbuf));

  printf("[espcli] Modo: solo-enviar (no se espera respuesta)\n");
  printf("[espcli] UART: %s\n", ESPCLI_TTY_PATH);
  printf("[espcli] Enviando: %s\n", txbuf);

  /* Abrir UART */

  fd = open(ESPCLI_TTY_PATH, O_RDWR | O_NOCTTY);
  if (fd < 0)
    {
      fprintf(stderr, "Error abriendo %s: %d\n\n", ESPCLI_TTY_PATH, errno);
      goto done;
    }

  /* Configurar UART a 115200 8N1 en modo raw */

  if (tcgetattr(fd, &tio) < 0)
    {
      fprintf(stderr, "tcgetattr fallo: %d\n\n", errno);
      goto done;
    }

  cfmakeraw(&tio);
  cfsetispeed(&tio, B115200);
  cfsetospeed(&tio, B115200);

  tio.c_cflag |= (CLOCAL | CREAD);
  tio.c_cflag &= ~PARENB;
  tio.c_cflag &= ~CSTOPB;
  tio.c_cflag &= ~CSIZE;
  tio.c_cflag |= CS8;
  tio.c_cflag &= ~CRTSCTS;

  /* Como ya no leemos nada, el timeout de lectura no nos importa,
   * pero dejamos algo razonable por si algun dia se reusa.
   */

  tio.c_cc[VTIME] = 0;
  tio.c_cc[VMIN]  = 0;

  if (tcsetattr(fd, TCSANOW, &tio) < 0)
    {
      fprintf(stderr, "tcsetattr fallo: %d\n\n", errno);
      goto done;
    }

  /* Enviar */

  ssize_t n = write(fd, txbuf, strlen(txbuf));
  if (n < 0)
    {
      fprintf(stderr, "Error en write: %d\n\n", errno);
      goto done;
    }

  tcdrain(fd);

  printf("[espcli] Comando enviado. No se esperara respuesta.\n\n");
  ret = EXIT_SUCCESS;

done:
  if (fd >= 0)
    {
      close(fd);
    }

  return ret;
}
