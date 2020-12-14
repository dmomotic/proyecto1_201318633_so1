package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"

	"github.com/rs/cors"
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

func main() {
	//Servidor
	mux := http.NewServeMux()

	//Ruta que retorna JSON con la informacion de la RAM
	mux.HandleFunc("/home", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Content-Type", "application/json")
		w.Write([]byte(getRAMInfo()))
	})

	//Ruta que retorna index.html
	mux.Handle("/", http.FileServer(http.Dir("../cliente/public")))

	//Servidor iniciado
	handler := cors.Default().Handler(mux)
	log.Println("Serving at localhost:8000...")
	log.Fatal(http.ListenAndServe(":8000", handler))
}
