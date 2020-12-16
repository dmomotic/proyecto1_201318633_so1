axios.defaults.baseURL = 'http://localhost:8000';

Vue.component("Proceso", Proceso);

new Vue({
  el: '#app',
  mounted() {
    //RAM
    this.chart = new ApexCharts(document.querySelector("#chart"), this.getChartOptions(['ram']))
    this.chart.render()
    //CPU
    this.chartCPU = new ApexCharts(document.querySelector("#chartCPU"), this.getChartOptions(['cpu1','cpu2','cpu3','cpu4']))
    this.chartCPU.render()
  },
  beforeDestory() {
    clearInterval(this.interval)
  },
  data() {
    return {
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
      cpu1: [],
      cpu2: [],
      cpu3: [],
      cpu4: [],
      chartCPU: null,
      watchingCPU: false
    }
  },
  methods: {
    getChartOptions(names) {
      return {
        chart: {
          type: 'line'
        },
        series: names.map((name) => ({name, data: []})),
        xaxis: {
          categories: []
        },
        chart: {
          height: '350px'
        }
      }
    },
    startRAMInfo() {
      this.watchingRam = true;
      this.setInterval()
    },
    stopRAMInfo() {
      this.watchingRam = false;
      this.setInterval()
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
    pushCPUUsage(value1, value2, value3, value4){
      //cpu1
      if(this.cpu1.length == 25) this.cpu1.splice(0,1)
      this.cpu1.push(value1)
      //cpu2
      if(this.cpu2.length == 25) this.cpu2.splice(0,1)
      this.cpu2.push(value2)
      //cpu3
      if(this.cpu3.length == 25) this.cpu3.splice(0,1)
      this.cpu3.push(value3)
      //cpu4
      if(this.cpu4.length == 25) this.cpu4.splice(0,1)
      this.cpu4.push(value4)
      //Updating
      this.chartCPU.updateSeries([
        {name: 'cpu1', data: this.cpu1},
        {name: 'cpu2', data: this.cpu2},
        {name: 'cpu3', data: this.cpu3},
        {name: 'cpu4', data: this.cpu4},
      ])
    },
    fetchCPUData() {
      axios.all([
        axios.get('/cpu'),
        axios.get('/cpu/usage'),
      ]).then(axios.spread((res1, res2) => {
        //Data from proc
        const info = res1.data;
        if (info != null) {
          this.cpu = info;
        } else {
          console.log('No se obtuvo la informacion del CPU')
        }
        //Data from go
        const usage = res2.data;
        this.pushCPUUsage(usage.cpu1, usage.cpu2, usage.cpu3, usage.cpu4)
      }))
    },
    setInterval() {
      clearInterval(this.interval)
      const time = 3000
      //cpu && ram
      if (this.watchingCPU && this.watchingRam) {
        this.interval = setInterval(() => {
          this.fetchRAMData()
          this.fetchCPUData()
        }, time)

      }
      //ram
      else if (this.watchingRam) {
        this.interval = setInterval(() => this.fetchRAMData(), time)
      }
      //cpu
      else if (this.watchingCPU) {
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
    }
  },
  computed: {
    stripedProgressBarRam() {
      return this.watchingRam ? 'progress-bar-striped' : '';
    }
  }
})