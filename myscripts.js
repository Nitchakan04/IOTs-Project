const apiURL = "http://<IP-ADDRESS>:<PORT>/api/sensors"; // URL API สำหรับดึงข้อมูลเซนเซอร์
const lightControlURL = "http://<IP-ADDRESS>:<PORT>/api/light-control"; // URL API สำหรับควบคุมไฟ

let lightStatus = false; // สถานะเริ่มต้นของไฟ (ปิด)

// ฟังก์ชันดึงข้อมูลจาก API
async function fetchSensorData() {
  try {
    const response = await fetch(apiURL);
    if (!response.ok) {
      throw new Error("ไม่สามารถดึงข้อมูลจาก API ได้");
    }
    const data = await response.json();

    // อัปเดตค่าบนหน้าเว็บ
    document.getElementById("temperature").textContent =
      data.temperature.toFixed(2);
    document.getElementById("tds").textContent = data.tds.toFixed(2);
    document.getElementById("light").textContent = data.light.toFixed(2);
    document.getElementById("water-level").textContent =
      data.water_level.toFixed(2);
    document.getElementById("ph").textContent = data.ph.toFixed(2);
  } catch (error) {
    console.error("ข้อผิดพลาด:", error);
  }
}

// ฟังก์ชันสลับสถานะไฟ
async function toggleLight() {
  try {
    const response = await fetch(lightControlURL, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ status: !lightStatus }), // สลับสถานะไฟ
    });

    if (!response.ok) {
      throw new Error("ไม่สามารถส่งคำสั่งไปยัง API ได้");
    }

    // อัปเดตสถานะไฟใหม่
    lightStatus = !lightStatus;

    // อัปเดตข้อความบนปุ่ม
    document.getElementById("light-toggle").textContent = lightStatus
      ? "ปิดไฟ"
      : "เปิดไฟ";
  } catch (error) {
    console.error("ข้อผิดพลาด:", error);
  }
}

// ดึงข้อมูลทุก 3 วินาที
setInterval(fetchSensorData, 3000);

// เพิ่ม Event Listener ให้ปุ่มควบคุมไฟ
document.getElementById("light-toggle").addEventListener("click", toggleLight);

// เรียกฟังก์ชันครั้งแรกเมื่อโหลดหน้า
fetchSensorData();
