axios.defaults.baseURL = 'http://localhost:8000';

var options ={
  chart: {
    type: 'line'
  },
  series: [{
    name: 'ram',
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

new Vue({
  el: '#app',
  mounted() {
    // this.interval = setInterval(() => this.fetchData(), 2500);
    this.chart = new ApexCharts(document.querySelector("#chart"), options)
    this.chart.render()
  },
  beforeDestory() {
    clearInterval()
  },
  data: {
    chart: null,
    seconds: 0,
    data: []
  },
  methods: {
    start(){
      this.interval = setInterval(() => this.fetchData(), 2500)
    },
    stop(){
      clearInterval(this.interval)
    },
    fetchData() {
      axios.get('/home').then((res) => {
        const info = res.data;
        this.push(info.memoriaEnUso)
      }).catch((e) => {
        console.log(e)
      })
    },
    push(value) {
      if(this.data.length == 25) this.data.splice(0,1)
      this.data.push(value)
      this.chart.updateSeries([{
        name: 'ram',
        data: this.data
      }])
    },
  }
})