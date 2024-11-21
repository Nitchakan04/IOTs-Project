// const apiURL = "http://<IP-ADDRESS>:<PORT>/api/sensors"; // URL API สำหรับดึงข้อมูลเซนเซอร์
// const lightControlURL = "http://<IP-ADDRESS>:<PORT>/api/light-control"; // URL API สำหรับควบคุมไฟ

// let lightStatus = false; // สถานะเริ่มต้นของไฟ (ปิด)

// // ฟังก์ชันดึงข้อมูลจาก API
// async function fetchSensorData() {
//   try {
//     const response = await fetch(apiURL);
//     if (!response.ok) {
//       throw new Error("ไม่สามารถดึงข้อมูลจาก API ได้");
//     }
//     const data = await response.json();

//     // อัปเดตค่าบนหน้าเว็บ
//     document.getElementById("temperature").textContent =
//       data.temperature.toFixed(2);
//     document.getElementById("tds").textContent = data.tds.toFixed(2);
//     document.getElementById("light").textContent = data.light.toFixed(2);
//     document.getElementById("water-level").textContent =
//       data.water_level.toFixed(2);
//     document.getElementById("ph").textContent = data.ph.toFixed(2);
//   } catch (error) {
//     console.error("ข้อผิดพลาด:", error);
//   }
// }

// // ฟังก์ชันสลับสถานะไฟ
// async function toggleLight() {
//   try {
//     const response = await fetch(lightControlURL, {
//       method: "POST",
//       headers: {
//         "Content-Type": "application/json",
//       },
//       body: JSON.stringify({ status: !lightStatus }), // สลับสถานะไฟ
//     });

//     if (!response.ok) {
//       throw new Error("ไม่สามารถส่งคำสั่งไปยัง API ได้");
//     }

//     // อัปเดตสถานะไฟใหม่
//     lightStatus = !lightStatus;

//     // อัปเดตข้อความบนปุ่ม
//     document.getElementById("light-toggle").textContent = lightStatus
//       ? "ปิดไฟ"
//       : "เปิดไฟ";
//   } catch (error) {
//     console.error("ข้อผิดพลาด:", error);
//   }
// }

// // ดึงข้อมูลทุก 3 วินาที
// setInterval(fetchSensorData, 3000);

// // เพิ่ม Event Listener ให้ปุ่มควบคุมไฟ
// document.getElementById("light-toggle").addEventListener("click", toggleLight);

// // เรียกฟังก์ชันครั้งแรกเมื่อโหลดหน้า
// fetchSensorData();


const apiURL = "http://<IP-ADDRESS>:<PORT>/api/sensors"; // URL API สำหรับดึงข้อมูลเซนเซอร์
const lightControlURL = "http://<IP-ADDRESS>:<PORT>/api/light-control"; // URL API สำหรับควบคุมไฟ

let lightStatuses = {
  "light-1": false,
  "light-2": false,
  "light-3": false,
  "light-4": false,
}; // สถานะเริ่มต้นของไฟแต่ละดวง

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

// // ฟังก์ชันสลับสถานะไฟดวงใดดวงหนึ่ง
// async function toggleLight(lightId) {
//   try {
//     const currentStatus = lightStatuses[lightId];
//     const response = await fetch(lightControlURL, {
//       method: "POST",
//       headers: {
//         "Content-Type": "application/json",
//       },
//       body: JSON.stringify({ id: lightId, status: !currentStatus }), // ส่งสถานะใหม่
//     });

//     if (!response.ok) {
//       throw new Error("ไม่สามารถส่งคำสั่งไปยัง API ได้");
//     }

//     // อัปเดตสถานะไฟใหม่
//     lightStatuses[lightId] = !currentStatus;

//     // อัปเดตข้อความหรือไอคอน
//     const button = document.getElementById(lightId);
//     const img = button.querySelector("img");
//     img.src = lightStatuses[lightId] ? "img/light2.png" : "img/light.png";
//     button.title = lightStatuses[lightId] ? "light2" : "light1";
//   } catch (error) {
//     console.error("ข้อผิดพลาด:", error);
//   }
// }
async function toggleLight(lightId) {
  try {
    // ตรวจสอบสถานะปัจจุบัน
    const currentStatus = lightStatuses[lightId];
    console.log(`สถานะไฟก่อนเปลี่ยน: ${lightId} = ${currentStatus}`);

    // ส่งคำขอ POST ไปที่ API
    const response = await fetch(lightControlURL, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({ id: lightId, status: !currentStatus }), // ส่งสถานะใหม่
    });

    if (!response.ok) {
      throw new Error("ไม่สามารถส่งคำสั่งไปยัง API ได้");
    }

    // อัปเดตสถานะใหม่
    lightStatuses[lightId] = !currentStatus;
    console.log(`สถานะไฟใหม่: ${lightId} = ${lightStatuses[lightId]}`);

    // อัปเดตข้อความหรือไอคอน
    const button = document.getElementById(lightId);
    const img = button.querySelector("img");

    // ตรวจสอบว่า img ถูกพบหรือไม่
    if (!img) {
      console.error(`ไม่พบ <img> ภายในปุ่ม ${lightId}`);
      return;
    }

    // เปลี่ยนแหล่งที่มาของรูปภาพ
    img.src = lightStatuses[lightId] ? "img/light2.png" : "img/light.png";
    console.log(`เปลี่ยนรูปภาพ: ${img.src}`);

    // เปลี่ยน title
    button.title = lightStatuses[lightId] ? "light2" : "light1";
  } catch (error) {
    console.error("ข้อผิดพลาด:", error);
  }
}

// ดึงข้อมูลทุก 3 วินาที
setInterval(fetchSensorData, 3000);

// เพิ่ม Event Listener ให้แต่ละปุ่ม
document.querySelectorAll(".light-icon").forEach((button) => {
  button.addEventListener("click", () => {
    toggleLight(button.id); // ส่ง id ของปุ่มที่ถูกกด
  });
});

// เรียกฟังก์ชันครั้งแรกเมื่อโหลดหน้า
fetchSensorData();
