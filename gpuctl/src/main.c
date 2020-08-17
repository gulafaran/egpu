/*
 * --------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tomenglund26@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. Tom Englund
 * --------------------------------------------------------------------------------
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <pci/pci.h>

typedef struct pci_st {
  struct pci_access *pciaccess;
  struct pci_dev *dev;
} pci_ctl;

void safefree(void **pp) {
  assert(pp);
  if(pp != NULL) {
    free(*pp);
    *pp = NULL;
  }
}

void gpu_init(pci_ctl *p_ctl) {
  p_ctl->pciaccess = pci_alloc();
  pci_init(p_ctl->pciaccess);
  pci_scan_bus(p_ctl->pciaccess);
}

void gpu_free(pci_ctl *p_ctl) {
  pci_cleanup(p_ctl->pciaccess);
}

bool gpu_isvga(u16 device_class) {
  if(device_class >= 768 && device_class <= 770) {
    return true;
  }
  else if(device_class == 896) {
    return true;
  }

  return false;
}

char *gpu_bus_str(u8 bus, u8 dev, u8 func) {
  char busbuf[100];
  sprintf(busbuf, "%d:%d:%d", bus, dev, func);
  char *str = strdup(busbuf);

  return str;
}

bool gpu_available(pci_ctl p_ctl, char *busid) {
  for(p_ctl.dev = p_ctl.pciaccess->devices; p_ctl.dev; p_ctl.dev = p_ctl.dev->next) {
    pci_fill_info(p_ctl.dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);
    if(gpu_isvga(p_ctl.dev->device_class)) {
      char *bus = gpu_bus_str(p_ctl.dev->bus, p_ctl.dev->dev, p_ctl.dev->func);
      if(strcmp(busid, bus) == 0) {
        safefree((void **)&bus);
        return true;
      }
      safefree((void **)&bus);
    }
  }
  return false;
}

void gpu_list(pci_ctl p_ctl) {
  char namebuf[1024];
  char classbuf[1024];

  for(p_ctl.dev = p_ctl.pciaccess->devices; p_ctl.dev; p_ctl.dev = p_ctl.dev->next) {
    pci_fill_info(p_ctl.dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);
    pci_lookup_name(p_ctl.pciaccess, namebuf, sizeof(namebuf), PCI_LOOKUP_DEVICE, p_ctl.dev->vendor_id, p_ctl    .dev->device_id);
    pci_lookup_name(p_ctl.pciaccess, classbuf, sizeof(classbuf), PCI_LOOKUP_CLASS, p_ctl.dev->device_class);
    if(gpu_isvga(p_ctl.dev->device_class)) {
      char *busid = gpu_bus_str(p_ctl.dev->bus, p_ctl.dev->dev, p_ctl.dev->func);
      fprintf(stdout, "busid: %s , name: [%s] (%s)\n", busid, classbuf, namebuf);
      safefree((void **)&busid);
    }
  }
}

void usage() {
  fprintf(stderr, "\n"
      "Usage: egpuctl [options]\n\n");
  fprintf(stderr,
      " Options:\n"
      "  -h, --help                     display this help and exit\n"
      "  -l, --list-gpus                print available gpus to stdout\n"
      "  -g, --gpu <busid>              check if gpu with busid is available and return EXIT_SUCCESS/EXITFAILURE\n");

}

int main(int argc, char **argv) {
  int opt, option_index = 0;
  int retval = EXIT_SUCCESS;

  static const struct option opts[] = {
    {"help",             no_argument,        0, 'h'},
    {"list-gpus",        no_argument,        0, 'l'},
    {"gpu",              required_argument,  0, 'g'},
    {NULL, 0, NULL, 0}
  };

  //malloc, Print detailed error message, stack trace, and memory mappings,
  //and abort the program on failure.
  mallopt(M_CHECK_ACTION, 3);

  while((opt = getopt_long(argc, argv, "g:hl", opts, &option_index)) != -1) {
    switch(opt) {
      case 'h':
        usage();
        goto clean;
      break;
      case 'l': {
        pci_ctl p_ctl;
        gpu_init(&p_ctl);
        gpu_list(p_ctl);
        gpu_free(&p_ctl);
      }
      break;
      case 'g': {
        pci_ctl p_ctl;
        gpu_init(&p_ctl);
        if(gpu_available(p_ctl, optarg)) {
            retval = EXIT_SUCCESS;
        }
        else {
          retval = EXIT_FAILURE;
        }
        gpu_free(&p_ctl);
        goto clean;
      }
      break;
      default:
        usage();
        retval = EXIT_FAILURE;
        goto clean;
    }
  }

  if(argc <= 1) {
    usage();
    retval = EXIT_FAILURE;
  }

clean:
  return retval;
}
