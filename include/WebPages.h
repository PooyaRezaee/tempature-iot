/**
 * @file WebPages.h
 * @brief HTML content stored in PROGMEM for the web server.
 */

#ifndef WEB_PAGES_H
#define WEB_PAGES_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"====(
    <!DOCTYPE html>
    <html lang="fa" dir="rtl"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
    <title>Temp Monitor</title>
    <style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:tahoma;background:#f0f2f5;padding:10px;min-height:100vh}
    .container{max-width:600px;margin:0 auto}.card{background:#fff;padding:20px;border-radius:12px;box-shadow:0 2px 8px rgba(0,0,0,0.1);margin-bottom:15px}
    .temp{font-size:clamp(2em,8vw,3.5em);color:#007bff;font-weight:bold;margin:15px 0}.clock{font-size:1.2em;color:#555;margin-bottom:10px}
    .status{padding:10px;border-radius:8px;margin:10px 0;font-weight:bold}.btn{display:block;width:100%;padding:12px;margin:8px 0;border:none;border-radius:8px;
    color:#fff;font-size:1em;cursor:pointer;transition:0.3s}.btn-blue{background:#007bff}.btn-blue:hover{background:#0056b3}
    .btn-green{background:#28a745}.btn-green:hover{background:#218838}.btn-red{background:#dc3545}.btn-red:hover{background:#c82333}
    .btn-orange{background:#fd7e14}.btn-orange:hover{background:#e8590c}.btn-purple{background:#6f42c1}.btn-purple:hover{background:#59359a}
    .info-box{background:#e7f3ff;padding:12px;border-radius:8px;margin:10px 0;font-size:0.9em}
    .settings{display:grid;gap:10px;margin:15px 0}.setting-row{display:flex;justify-content:space-between;align-items:center;padding:10px;background:#f8f9fa;border-radius:6px}
    .toggle{position:relative;width:50px;height:26px;background:#ccc;border-radius:13px;cursor:pointer;transition:0.3s}.toggle.on{background:#28a745}
    .toggle::after{content:'';position:absolute;width:22px;height:22px;background:#fff;border-radius:50%;top:2px;left:2px;transition:0.3s}
    .toggle.on::after{left:26px}@media(max-width:480px){.card{padding:15px}.temp{font-size:2.5em}}</style></head>
    <body><div class="container"><div class="card"><h2 style="text-align:center;color:#333;margin-bottom:15px">سیستم پایش دما</h2>
    <div class="clock" id="clock">--:--:--</div><div class="temp"><span id="t">--</span>°C</div><div id="stat" class="status"></div>
    <div class="info-box">داده ها: <strong id="logCount">0</strong> | حجم: <strong id="logSize">0</strong> KB</div>
    <div class="settings"><div class="setting-row"><span>صدای هشدار وب</span><div class="toggle" id="soundToggle" onclick="toggleSound()"></div></div></div>
    <button class="btn btn-purple" onclick="toggleDisplay()">🔄 تغییر نمایشگر (دما/ساعت)</button>
    <button class="btn btn-blue" onclick="location.href='/limits'">تنظیم دما</button>
    <button class="btn btn-green" onclick="location.href='/history'">نمودار</button>
    <button class="btn btn-orange" onclick="location.href='/set_time'">تنظیم ساعت</button>
    <button class="btn btn-red" onclick="clearLog()">پاکسازی حافظه</button></div></div>
    <script>
    function update(){fetch('/status').then(r=>r.json()).then(d=>{document.getElementById('t').innerText=d.temp;
    document.getElementById('clock').innerText=d.time;document.getElementById('logCount').innerText=d.logCount;
    document.getElementById('logSize').innerText=d.logSize;const st=document.getElementById('stat');
    const snd=document.getElementById('soundToggle');if(d.webSound){snd.classList.add('on')}else{snd.classList.remove('on')}
    if(d.temp<=d.green){st.innerHTML='وضعیت نرمال';st.style.background='#d4edda';st.style.color='#155724'}
    else if(d.temp<=d.yellow){st.innerHTML='هشدار';st.style.background='#fff3cd';st.style.color='#856404'}
    else{st.innerHTML='خطر';st.style.background='#f8d7da';st.style.color='#721c24'}}).catch(e=>console.log(e))}
    function toggleSound(){fetch('/toggle_sound').then(()=>update())}
    function toggleDisplay(){fetch('/toggle_disp')}
    function clearLog(){if(confirm('مطمئن هستید؟')){fetch('/clear_log').then(()=>{alert('انجام شد');update()})}}
    setInterval(update,1000);update();
    </script></body></html>
    )====";

const char LIMITS_HTML[] PROGMEM = R"====(
    <!DOCTYPE html>
    <html lang="fa" dir="rtl"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
    <title>Limit Config</title>
    <style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:tahoma;background:#f0f2f5;padding:10px}
    .container{max-width:500px;margin:0 auto}.card{background:#fff;padding:20px;border-radius:12px;box-shadow:0 2px 8px rgba(0,0,0,0.1)}
    h3{color:#333;margin-bottom:20px;text-align:center}.form-group{margin:15px 0}label{display:block;margin-bottom:5px;color:#555;font-weight:bold}
    input{width:100%;padding:10px;border:2px solid #ddd;border-radius:6px;font-size:1em}input:focus{outline:none;border-color:#007bff}
    .btn{width:100%;padding:12px;margin:10px 0;border:none;border-radius:8px;font-size:1em;cursor:pointer;color:#fff;transition:0.3s}
    .btn-save{background:#28a745}.btn-save:hover{background:#218838}.btn-back{background:#6c757d}.btn-back:hover{background:#545b62}
    .current{background:#e7f3ff;padding:10px;border-radius:6px;margin-bottom:15px;text-align:center}</style></head>
    <body><div class="container"><div class="card"><h3>تنظیم حدود دما</h3>
    <div class="current" id="current">...</div>
    <form onsubmit="save(event)"><div class="form-group"><label>حد سبز:</label>
    <input type="number" step="0.1" id="green" required></div>
    <div class="form-group"><label>حد زرد:</label><input type="number" step="0.1" id="yellow" required></div>
    <button type="submit" class="btn btn-save">ذخیره</button></form>
    <button class="btn btn-back" onclick="location.href='/'">بازگشت</button></div></div>
    <script>
    fetch('/get_limits').then(r=>r.json()).then(d=>{document.getElementById('green').value=d.green;
    document.getElementById('yellow').value=d.yellow;document.getElementById('current').innerHTML=
    'فعلی: سبز '+d.green+' | زرد '+d.yellow});
    function save(e){e.preventDefault();const g=document.getElementById('green').value;
    const y=document.getElementById('yellow').value;if(parseFloat(y)<=parseFloat(g)){alert('خطا در مقادیر');return}
    fetch('/save_limits?green='+g+'&yellow='+y).then(()=>{alert('ذخیره شد');location.href='/'})}
    </script></body></html>
    )====";

const char SET_TIME_HTML[] PROGMEM = R"====(
    <!DOCTYPE html>
    <html lang="fa" dir="rtl"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
    <title>Time Config</title>
    <style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:tahoma;background:#f0f2f5;padding:10px}
    .container{max-width:500px;margin:0 auto}.card{background:#fff;padding:20px;border-radius:12px;box-shadow:0 2px 8px rgba(0,0,0,0.1)}
    h3{color:#333;margin-bottom:20px;text-align:center}.current-time{background:#e7f3ff;padding:15px;border-radius:8px;margin-bottom:20px;text-align:center}
    .time-display{font-size:1.5em;font-weight:bold;color:#007bff;margin:10px 0}input{width:100%;padding:12px;border:2px solid #ddd;
    border-radius:6px;font-size:1em;margin:10px 0}input:focus{outline:none;border-color:#007bff}.btn{width:100%;padding:12px;
    margin:8px 0;border:none;border-radius:8px;font-size:1em;cursor:pointer;color:#fff;transition:0.3s}.btn-save{background:#28a745}
    .btn-save:hover{background:#218838}.btn-back{background:#6c757d}.btn-back:hover{background:#545b62}</style></head>
    <body><div class="container"><div class="card"><h3>تنظیم ساعت</h3>
    <div class="current-time"><div>ساعت دستگاه:</div><div class="time-display" id="currentTime">--</div></div>
    <form action="/save_time"><input type="datetime-local" name="dt" required>
    <button type="submit" class="btn btn-save">ذخیره</button></form>
    <button class="btn btn-back" onclick="location.href='/'">بازگشت</button></div></div>
    <script>function update(){fetch('/status').then(r=>r.json()).then(d=>{
    document.getElementById('currentTime').innerText=d.date+' '+d.time})}setInterval(update,1000);update();</script>
    </body></html>
    )====";

const char HISTORY_HTML[] PROGMEM = R"====(
    <!DOCTYPE html>
    <html lang="fa" dir="rtl"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
    <title>History</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:tahoma;background:#f0f2f5;padding:10px}
    .container{max-width:900px;margin:0 auto}.card{background:#fff;padding:15px;border-radius:12px;box-shadow:0 2px 8px rgba(0,0,0,0.1);
    margin-bottom:15px}h3{color:#333;text-align:center;margin-bottom:15px}select{width:100%;padding:10px;border:2px solid #ddd;
    border-radius:6px;font-size:1em;margin-bottom:15px}canvas{max-width:100%;height:auto!important}.btn{display:block;width:100%;
    padding:12px;margin:10px 0;border:none;border-radius:8px;background:#6c757d;color:#fff;font-size:1em;cursor:pointer}
    .btn:hover{background:#545b62}@media(max-width:768px){.card{padding:10px}}</style></head>
    <body><div class="container"><div class="card"><h3>تاریخچه دما</h3>
    <select id="m" onchange="load()"><option value="1">1 ساعت</option>
    <option value="2">امروز</option><option value="3">کل</option></select>
    <canvas id="c"></canvas><button class="btn" onclick="location.href='/'">بازگشت</button></div></div>
    <script>let chart;
    function load(){const m=document.getElementById('m').value;fetch('/get_hist?m='+m).then(r=>r.json()).then(d=>{
    const ctx=document.getElementById('c').getContext('2d');if(chart)chart.destroy();
    chart=new Chart(ctx,{type:'line',data:{labels:d.map(i=>i.l),datasets:[{label:'Temp',data:d.map(i=>i.t),
    borderColor:'#007bff',backgroundColor:'rgba(0,123,255,0.1)',fill:true,tension:0.3,pointRadius:4,pointHoverRadius:6}]},
    options:{responsive:true,maintainAspectRatio:true,plugins:{tooltip:{callbacks:{title:function(ctx){return d[ctx[0].dataIndex].f},
    label:function(ctx){return 'Temp: '+ctx.parsed.y.toFixed(1)}}}},scales:{y:{beginAtZero:false}}}});})}load();</script></body></html>
    )====";

#endif // WEB_PAGES_H