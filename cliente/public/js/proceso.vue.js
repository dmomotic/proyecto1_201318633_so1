axios.defaults.baseURL = 'http://localhost:8000';

const Proceso = {
  template: /*template*/`
    <div>
    <ul class="list-group" data-bs-toggle="collapse" :data-bs-target="selector" aria-expanded="false" :aria-controls="customId">
      <li class="list-group-item">
        <p class="fw-bold">
          {{ id }} - {{ proceso }} - {{ status }} ({{ hijos.length }})
          <div class="col">
            <button type="button" class="btn btn-danger btn-sm" @click.prevent="killProcess(id)">Kill</button>
          </div>
        </p>
      </li>
      <ul class="list-group collapse" :id="customId">
        <li class="list-group-item" v-for="(item, index) in hijos" :key="index">
        {{ item.id }} - {{ item.proceso }}
        </li>
      </ul>
    </ul>
    </div>
  `,
  props: [
    'proceso',
    'id',
    'status',
    'hijos'
  ],
  data() {
    return {
      customId: 'collapsable' + this.id,
      selector: '#collapsable' + this.id
    }
  },
  methods: {
    killProcess(pid){
      axios.get('/process/kill', {params: {pid}});
    }
  }
}