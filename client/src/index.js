const baseUrl = '';
let temp = 68;
let thermo = 65;
$('#slider').roundSlider({
  min: 50,
  max: 90,
  radius: 85,
  sliderType: 'default',
  value: thermo,
  startAngle: 270,
  editableTooltip: false,
  stop: showValue
});
let pwrState = true,
  htrState = false,
  tempF = 0;
const clsPwrOn = 'btn-power--on';
const clsPwrOff = 'btn-power--off';
const evt = 'click';
const tempEl = $('#temp');
const power = $('#power');
power.addClass(clsPwrOn);
power.on(evt, togglePwr);
const heater = $('#heater');
heater.addClass(clsPwrOff);
heater.on(evt, toggleHtr);
function togglePwr() {
  if (pwrState) {
    axios.get(baseUrl + '/power/off').then(
      function(data) {
        swapClass(power, clsPwrOff, clsPwrOn);
        swapClass(heater, clsPwrOff, clsPwrOn);
      }.bind(this)
    );
  } else {
    axios.get(baseUrl + '/power/on').then(
      function(data) {
        swapClass(power, clsPwrOn, clsPwrOff);
      }.bind(this)
    );
  }
  pwrState = !pwrState;
}
function toggleHtr(state) {
  if (htrState) {
    axios.get(baseUrl + '/heater/off').then(
      function() {
        swapClass(heater, clsPwrOff, clsPwrOn);
      }.bind(this)
    );
  } else if (pwrState) {
    axios.get(baseUrl + '/heater/on').then(
      function() {
        swapClass(heater, clsPwrOn, clsPwrOff);
      }.bind(this)
    );
  }
  htrState = pwrState ? !htrState : htrState;
}
function swapClass(el, add, remove) {
  el.addClass(add);
  el.removeClass(remove);
}
function showValue(slider) {
  axios.post(baseUrl + `/heater/temp?temp=${slider.value}`).then(
    function() {
      if (slider.value < tempF) {
        swapClass(heater, clsPwrOn, clsPwrOff);
      } else {
        swapClass(heater, clsPwrOff, clsPwrOn);
      }
    }.bind(this)
  );
}
axios.get(baseUrl + '/heater/temp').then(function(res) {
  tempEl.text(res.data.temp);
  tempF = res.data.temp;
});
