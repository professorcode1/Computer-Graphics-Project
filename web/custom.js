function activate_update_button(){
    document.getElementById("update").removeAttribute("disabled");
}
function set_up_slider(slider_id, start, step, range,valud_id, is_int){
    const slider = document.getElementById(slider_id);
    noUiSlider.create(slider, {
        start: [start], 
        step, 
        range,
        

    });
    let isInitialUpdate = true; // Flag to track the initial update
    // Event listener to handle value changes
    slider.noUiSlider.on('update', function (values, handle) {
        // console.log('Slider Value:', values[handle]);
        if(isInitialUpdate){
            isInitialUpdate = false;
        }else{
            activate_update_button();
        }
        if(is_int){
            document.getElementById(valud_id).innerText = `(${Math.floor(values[handle])})`
        }else{
            document.getElementById(valud_id).innerText = `(${Number(values[handle]).toFixed(2)})`
        }
    });
}
function setup_division_slider(){
    set_up_slider(
        "division-slider",
        10,
        1,
        {
            'min': [10], 
            'max': [100] 
        },
        "division-value",
        true
    );    
}
function setBit(number, bitPosition) {
    return number | (1 << bitPosition);
}
function clearBit(number, bitPosition) {
    return number & ~(1 << bitPosition);
}
function create_active_bit_index_array_string(number){
    const res = [];
    for(let i=0 ; i<32 ; i++){
        if(number & (1 << i)) res.push(i+1);
    }
    return res.map(String).join(",")
}
function setup_active_wave_numbers_input(){
    const container = document.getElementById('active-wave-numbers-input-container');
    const checkboxes = container.querySelectorAll('input[type="checkbox"]');
    const active_wave_numbers_values_ele = document.getElementById("active-wave-numbers-values");
    var active_wave_numbers_binary_rep = 1023;
    active_wave_numbers_values_ele.innerHTML = "(1,2,3,4,5,6,7,8,9,10)"
    checkboxes.forEach((checkbox, index) => {
        checkbox.addEventListener('change', () => {
            activate_update_button()
            if(checkbox.checked){
                active_wave_numbers_binary_rep = setBit(active_wave_numbers_binary_rep, index)
            }else{
                active_wave_numbers_binary_rep = clearBit(active_wave_numbers_binary_rep, index)
            }
            active_wave_numbers_values_ele.innerHTML = `(${create_active_bit_index_array_string(active_wave_numbers_binary_rep)})`;
        });
    });

}
function setup_rotation_angle(){
    set_up_slider("rotation-slider", 36.87, 0.01, {
        "min":0,
        "max":180
    }, "rotation-angle-value", false);
}
function setup_remaing_parameters(){
    [
        ['output-increase-fctr',3.0],
        ['input-shrink-fctr',3.0],
        ['lacunarity', 2.0],
        ['persistance', 3.1]
    ].map(([parameter_name, parameter_value]) => {
        set_up_slider(
            `${parameter_name}-slider`,
            parameter_value,
            0.1,
            {
                'min': [1], 
                'max': [10] 
            },
            `${parameter_name}-value`,
            false
        )
    });
}
function setup_plane_change(){
    const plane_buttons = document.getElementById("plane-selector").querySelectorAll('button');
    plane_buttons.forEach((button, index,arr) => {
        button.addEventListener("click", (event)=>{
            plane_buttons.forEach(x => x.disabled = false);
            event.target.disabled = true;
            _change_plane(Number(event.target.innerText))
        })
    })
}
setup_active_wave_numbers_input();
setup_rotation_angle();
setup_remaing_parameters();
setup_plane_change()
document.getElementById("update").addEventListener("click", (event)=>{
    
    const arguments_to_update_terrain = [
    "active-wave-numbers-values",
    "rotation-angle-value",
    "output-increase-fctr-value",
    "input-shrink-fctr-value",
    "lacunarity-value",
    "persistance-value"].map(
        x => document.getElementById(x).innerText.replace('(','').replace(')','')
    ).map(x => {
        if(x.includes(',')){
            return x.split(',').map(Number).map(x => x-1).reduce(setBit, 0)
        }else{
            return Number(x)
        }
    });
    _change_terrain_parameters(10, ...arguments_to_update_terrain)
})