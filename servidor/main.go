package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/exec"
	"time"

	"github.com/rs/cors"
	"github.com/shirou/gopsutil/cpu"
)

//Obtengo la informacion de la RAM
func getRAMInfo() string {
	filedata, err := ioutil.ReadFile("/proc/memo_201318633")
	if err != nil {
		fmt.Println("Error al abrir memo_201318633")
		return ""
	}
	return string(filedata)
}

//Obtengo la informacion del CPU desde el proc
func getCPUInfo() string {
	filedata, err := ioutil.ReadFile("/proc/cpu_201318633")
	if err != nil {
		fmt.Println("Error al abrir cpu_201318633")
		return ""
	}
	return string(filedata)
}

//Obtengo el porcentaje de uso del CPU con la ayuda de GO
func getCPUUsage() string {
	cpuPercents, err := cpu.Percent(time.Second, true)
	if err != nil {
		return fmt.Sprintf("{\"cpu1\":%.2f,\"cpu2\":%.2f,\"cpu3\":%.2f,\"cpu4\":%.2f}", 0.00, 0.00, 0.00, 0.00)
	}
	//fmt.Printf("logical cpus %v \n", cpuPercents)
	return fmt.Sprintf("{\"cpu1\":%.2f,\"cpu2\":%.2f,\"cpu3\":%.2f,\"cpu4\":%.2f}", cpuPercents[0], cpuPercents[1], cpuPercents[2], cpuPercents[3])
}

func killByPID(pid string) {
	output, err := exec.Command("kill", "-9", pid).CombinedOutput()
	if err != nil {
		os.Stderr.WriteString(err.Error())
	}
	fmt.Println(string(output))
}

func main() {
	//Servidor
	mux := http.NewServeMux()

	//Ruta que retorna JSON con la informacion de la RAM
	mux.HandleFunc("/ram", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		w.Write([]byte(getRAMInfo()))
	})

	//Ruta que retorna JSON con la informacion del CPU
	mux.HandleFunc("/cpu", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		w.Write([]byte(getCPUInfo()))
	})

	//Ruta que retorna JSON con la informacion del CPU
	mux.HandleFunc("/cpu/usage", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		w.Write([]byte(getCPUUsage()))
	})

	mux.HandleFunc("/process/kill", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")

		//r.RequestURI = /process/kill?pid=numero
		pid := r.RequestURI[18:]
		killByPID(pid)
		w.Write([]byte("{message:\"All done\"}"))
	})

	//Ruta que retorna index.html
	mux.Handle("/", http.FileServer(http.Dir("../cliente/public")))

	//Servidor iniciado
	handler := cors.Default().Handler(mux)
	log.Println("Serving at localhost:8000...")
	log.Fatal(http.ListenAndServe(":8000", handler))
}
