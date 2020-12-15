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
    // sprintf(buffer, "UNKNOWN:%ld\n", state);
    // return buffer;
    return "UNKNOWN";
  }
}

static int write_and_read(struct seq_file *file, void *v)
{
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
    // seq_printf(file,"\"status\":%ld,", task_list->state);
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
