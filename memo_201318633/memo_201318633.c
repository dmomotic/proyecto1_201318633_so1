#include <linux/init.h>        
#include <linux/module.h>   
#include <linux/kernel.h>    
#include <linux/seq_file.h> 
#include <linux/hugetlb.h> //si_meminfo
#include <linux/proc_fs.h> //proc_create
#include <linux/sys.h>


#define BUFSIZE  150

// Documentacion del modulo
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modulo de memoria SYSINFO");
MODULE_AUTHOR("Diego Antonio Momotic Montesdeoca");
MODULE_VERSION("1.0");

struct sysinfo systemInfo;
 
static int write_and_read(struct seq_file *file, void *v){
    unsigned long total_memoria, memoria_libre, memoria_uso, porcentaje_uso;
    
    si_meminfo(&systemInfo);

    total_memoria = (systemInfo.totalram * systemInfo.mem_unit) / (1024 * 1024);
    memoria_libre = (systemInfo.freeram * systemInfo.mem_unit) / (1024 * 1024);
    memoria_uso = total_memoria - memoria_libre;
    porcentaje_uso = (100 * memoria_uso) / total_memoria;
    // seq_printf(file,"MEMORIA TOTAL: %lu MB\n", total_memoria);
    // seq_printf(file,"MEMORIA LIBRE: %lu MB\n", memoria_libre);
    // seq_printf(file,"MEMORIA EN USO: %lu MB\n", memoria_uso);
    // seq_printf(file,"PORCENTAJE: %lu %%\n", (100 * memoria_uso) / total_memoria);

    //Lo guardo como JSON para que en el servidor sea mas facil la lectura del archivo
    seq_printf(file,"{");
    seq_printf(file,"\"memoriaTotal\":%lu,", total_memoria);
    seq_printf(file,"\"memoriaLibre\":%lu,", memoria_libre);
    seq_printf(file,"\"memoriaEnUso\":%lu,", memoria_uso);
    seq_printf(file,"\"porcentajeEnUso\":%lu", porcentaje_uso);
    seq_printf(file,"}");
    return 0;
}

static int open_file(struct inode *inode, struct  file *file) {
  return single_open(file, write_and_read, NULL);
}

static struct file_operations fops =
{    
    .open = open_file,
    .read = seq_read
};

static int __init inicio(void)
{
    proc_create("memo_201318633", 0, NULL, &fops);
    printk(KERN_ALERT "                   MODULO INSERTADO                \n");
    printk(KERN_ALERT "                   Carnet 201318633                \n\n");
    return 0;
}

static void __exit salida(void)
{
	remove_proc_entry("memo_201318633",NULL);
    printk(KERN_ALERT "                  MODULO REMOVIDO                  \n");
    printk(KERN_ALERT "               SISTEMAS OPERATIVOS 1               \n\n");
}

module_init(inicio);
module_exit(salida);
