axios.defaults.baseURL = 'http://localhost:8000';

new Vue({
  el: '#app',
  mounted() {
    this.chart = new ApexCharts(document.querySelector("#chart"), this.getChartOptions('ram'))
    this.chart.render()
  },
  beforeDestory() {
    clearInterval()
  },
  data: {
    chart: null,
    seconds: 0,
    data: [],
    ramUsage: 0,
    totalRam: 0,
    watchingRam: false,
    cpu: {
      procesos: [], //{id, proceso, status, hijos}
      total: 0,
      ejecucion: 0,
      suspendidos: 0,
      detenidos: 0,
      zombies: 0, 
      desconocido: 0
    },
    chartCPU: null,
    watchingCPU: false
  },
  methods: {
    getChartOptions(name) {
      return {
        chart: {
          type: 'line'
        },
        series: [{
          name: name,
          data: []
        }],
        xaxis: {
          categories: []
        },
        stroke: {
          curve: 'smooth'
        },
        chart: {
          height: '350px'
        }
      }
    },
    startRAMInfo() {
      this.watchingRam = true;
      this.setInterval()
      // this.interval = setInterval(() => this.fetchRAMData(), 2500)
    },
    stopRAMInfo() {
      this.watchingRam = false;
      this.setInterval()
      // clearInterval(this.interval)
    },
    fetchRAMData() {
      axios.get('/ram').then((res) => {
        const info = res.data;
        this.pushRAMValue(info.memoriaEnUso)
        this.ramUsage = info.porcentajeEnUso
        this.totalRam = info.memoriaTotal
      }).catch((e) => {
        console.log(e)
      })
    },
    pushRAMValue(value) {
      if (this.data.length == 25) this.data.splice(0, 1)
      this.data.push(value)
      this.chart.updateSeries([{
        name: 'ram',
        data: this.data
      }])
    },
    fetchCPUData(){
      axios.get('/cpu').then((res) => {
        const info = res.data;
        if(info != null){
          this.cpu = info;
        }else{
          console.log('No se obtuvo la informacion del CPU')
        }
      }).catch((e) => {
        console.log(e)
      })
    },
    setInterval(){
      clearInterval(this.interval)
      const time = 2500
      //cpu && ram
      if(this.watchingCPU && this.watchingRam){
        this.interval = setInterval(() => {
          this.fetchRAMData()
          this.fetchCPUData()
        }, time)
        
      }
      //ram
      else if(this.watchingRam){
        this.interval = setInterval(() => this.fetchRAMData(), time)
      }
      //cpu
      else if(this.watchingCPU){
        this.interval = setInterval(() => this.fetchCPUData(), time)
      }
    },
    startCPUInfo() {
      this.watchingCPU = true;
      this.setInterval()
    },
    stopCPUInfo() {
      this.watchingCPU = false;
      this.setInterval()
    },
  },
  computed: {
    stripedProgressBarRam() {
      return this.watchingRam ? 'progress-bar-striped' : '';
    }
  }
})