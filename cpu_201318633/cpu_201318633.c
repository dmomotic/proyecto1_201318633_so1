#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/hugetlb.h>
#include <linux/fs.h>
#include <linux/sched/signal.h>

// Documentacion del modulo
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modulo de CPU task_struct");
MODULE_AUTHOR("Diego Antonio Momotic Montesdeoca");
MODULE_VERSION("1.0");

struct task_struct *task_list;
struct task_struct *task_child;
struct list_head *list;

char buffer[256];

//Contadores
unsigned int total = 0;
unsigned int ejecucion = 0;
unsigned int suspendidos = 0;
unsigned int detenidos = 0;
unsigned int zombies = 0;
unsigned int desconocido = 0;

int contador1 = 0;
int contador2 = 0;

unsigned long ram = 0;

char *get_task_state(long state)
{
  total++;
  switch (state)
  {
  case TASK_RUNNING:
    ejecucion++;
    return "RUNNING";
  case TASK_INTERRUPTIBLE:
    suspendidos++;
    return "SLEEPING";
  case 4:
    zombies++;
    return "ZOMBIE";
  case 8:
    detenidos++;
    return "STOPPED";
  case 1026:
    suspendidos++;
    return "SLEEPING";
  default:
    desconocido++;
    return "UNKNOWN";
  }
}

//getent passwd
char *get_owner_of_process(int code){
  switch (code)
  {
  case 0: 
    return "root";
  case 1:
    return "daemon";
  case 2:
    return "bin";
  case 3:
    return "diego";
  case 4:
    return "sync";
  case 5:
    return "games";
  case 6:
    return "man";
  case 7:
    return "lp";
  case 8:
    return "mail";
  case 9:
    return "news";
  case 10:
    return "uucp";
  case 13:
    return "proxy";
  case 33:
    return "www-data";
  case 34:
    return "backup";
  case 38:
    return "list";
  case 39:
    return "irc";
  case 41:
    return "gnats";
  case 65534:
    return "nobody";
  case 100:
    return "systemd-network";
  case 101:
    return "systemd-resolve";
  case 102:
    return "systemd-timesync";
  case 103:
    return "messagebus";
  case 104:
    return "syslog";
  case 105:
    return "_apt";
  case 106:
    return "tss";
  case 107:
    return "uuidd";
  case 108:
    return "tcpdump";
  case 109:
    return "avahi-autoipd";
  case 110:
    return "usbmux";
  case 111:
    return "rtkit";
  case 112:
    return "dnsmasq";
  case 113:
    return "cups-pk-helper";
  case 114:
    return "speech-dispatcher";
  case 115:
    return "avahi";
  case 116:
    return "kernoops";
  case 117:
    return "saned";
  case 118:
    return "nm-openvpn";
  case 119:
    return "hplip";
  case 120:
    return "whoopsie";
  case 121:
    return "colord";
  case 122:
    return "geoclue";
  case 123:
    return "pulse";
  case 124:
    return "gnome-initial-setup";
  case 125:
    return "gmd";
  case 1000:
    return "diego";
  case 999:
    return "systemd-coredump";
  default:
    return "root";
  }
}

static int write_and_read(struct seq_file *file, void *v)
{
  ram = 0;

  total = 0;
  ejecucion = 0;
  suspendidos = 0;
  detenidos = 0;
  zombies = 0;
  desconocido = 0;

  contador1 = 0;

  //Lo guardo como JSON para que en el servidor sea mas facil la lectura del archivo
  seq_printf(file, "{");
  seq_printf(file, "\"procesos\":"); //Atributo procesos
  seq_printf(file, "["); //Inicio de arreglo procesos
  for_each_process(task_list)
  {
    if(contador1 > 0){
      seq_printf(file, ",");  
    }
    seq_printf(file, "{"); 
    seq_printf(file,"\"id\":%d,", task_list->pid); 
    seq_printf(file,"\"proceso\":\"%s\",", task_list->comm); 
    if(task_list->mm){
      ram = (get_mm_rss(task_list->mm) * 4096) / (1024 * 1024); //systemInfo.mem_unit = 4096
    }
    seq_printf(file,"\"ram\":\"%lu\",", ram);
    seq_printf(file,"\"porcentaje\":\"%lu\",", ram * 100 / 11896);  //systemInfo.totalram = 11896
    seq_printf(file,"\"proceso\":\"%s\",", task_list->comm);
    seq_printf(file,"\"usuario\":\"%s\",", get_owner_of_process((int) task_list->sessionid));
    seq_printf(file,"\"status\":\"%s\",", get_task_state(task_list->state));
    
    seq_printf(file, "\"hijos\":"); 
    seq_printf(file, "["); 
    contador2 = 0;
    list_for_each(list, &task_list->children)
    {
      if(contador2 > 0){
        seq_printf(file, ",");  
      }
      task_child = list_entry(list, struct task_struct, sibling);
      seq_printf(file, "{");
      seq_printf(file,"\"id\":%d,", task_child->pid); 
      seq_printf(file,"\"proceso\":\"%s\",", task_child->comm); 
      seq_printf(file,"\"status\":\"%s\"", get_task_state(task_child->state));
      seq_printf(file, "}");
      contador2++;
    }
    seq_printf(file, "]");
    seq_printf(file, "}");
    contador1++;
  }
  seq_printf(file, "],"); //Cierre arreglo procesos
  seq_printf(file,"\"total\":%d,", total);
  seq_printf(file,"\"ejecucion\":%d,", ejecucion);
  seq_printf(file,"\"suspendidos\":%d,", suspendidos);
  seq_printf(file,"\"detenidos\":%d,", detenidos);
  seq_printf(file,"\"zombies\":%d,", zombies);
  seq_printf(file,"\"desconocido\":%d", desconocido);  
  seq_printf(file, "}"); //Cierre objeto global
  return 0;
}

static int open_file(struct inode *inode, struct  file *file) {
  return single_open(file, write_and_read, NULL);
}

static struct file_operations fcpu =
{    
  .open = open_file,
  .read = seq_read
}; 

static int inicio(void)
{
  proc_create("cpu_201318633", 0, NULL, &fcpu);
  printk(KERN_ALERT "                   MODULO INSERTADO                \n");
  printk(KERN_ALERT "          Diego Antonio Momotic Montesdeoca        \n\n");
  return 0;
}

static void salida(void)
{
  remove_proc_entry("cpu_201318633",NULL);
  printk(KERN_ALERT "                  MODULO REMOVIDO                  \n");
  printk(KERN_ALERT "                   DICIEMBRE 2020                  \n\n");
}

module_init(inicio);
module_exit(salida);

