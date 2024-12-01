// Replace with your Blynk Auth Token
const authToken = "nPlod4N5_OwUKZmhRstb_9BvLH6OIsJQ";

// Map virtual pins to their corresponding element IDs
const blynkDataMapping = {
  "water-level": "v0",
  light: "v1",
  temperature: "v2",
  tds: "v3",
};

// Fetch data from Blynk API
function fetchBlynkData() {
  for (const [key, pin] of Object.entries(blynkDataMapping)) {
    const apiUrl = `https://blynk.cloud/external/api/get?token=${authToken}&${pin}`;

    fetch(apiUrl)
      .then((response) => response.text())
      .then((data) => {
        document.getElementById(key).innerText = data;
      })
      .catch((error) => {
        console.error(`Error fetching data for ${key}:`, error);
      });
  }
}


// Fetch the last row from Google Sheets (Column C)
async function fetchGoogleSheetData() {
  const apiKey = "YOUR_GOOGLE_API_KEY"; // ใส่ API Key ที่ได้จาก Google
  const spreadsheetId = "https://script.google.com/macros/s/AKfycbxWHcuErArFnfKnWIhJ8Kj4ZLaj0oCDnYoESF_VHLiS2ylDMPVl8g5I3g0J91RsDEb1Pg/exec"; // ใส่ Spreadsheet ID
  const range = "C:C"; // ระบุช่วงคอลัมน์ C

  const url = `https://sheets.googleapis.com/v4/spreadsheets/${spreadsheetId}/values/${range}?key=${apiKey}`;
  try {
      const response = await fetch(url);
      const data = await response.json();
      const values = data.values;

      if (values && values.length > 0) {
          // ดึงข้อมูลจากแถวล่างสุดของคอลัมน์ C
          const lastRow = values[values.length - 1][0];
          document.getElementById("animals").innerText = lastRow;
      } else {
          console.error("No data found in the specified range.");
      }
  } catch (error) {
      console.error("Error fetching Google Sheets data:", error);
  }
}


 // Fetch data on page load
 window.onload = () => {
  fetchBlynkData();
  fetchGoogleSheetData();
};

// Refresh Blynk data every 5 seconds
setInterval(fetchBlynkData, 5000);

//----------------------------------------------------------------------------------------------------------
// ฟังก์ชันเปลี่ยนสถานะไฟ
// async function toggleLight(lightId) {
//     try {
//         const currentStatus = lightStatuses[lightId]; // สถานะปัจจุบัน
//         console.log(`สถานะไฟก่อนเปลี่ยน: ${lightId} = ${currentStatus}`);

//         // ส่งคำสั่งไปยัง API
//         const response = await fetch(lightControlURL, {
//             method: "POST",
//             headers: {
//                 "Content-Type": "application/json",
//             },
//             body: JSON.stringify({ id: lightId, status: !currentStatus }),
//         });

//         if (!response.ok) {
//             throw new Error("ไม่สามารถส่งคำสั่งไปยัง API ได้");
//         }

//         // อัปเดตสถานะใหม่
//         lightStatuses[lightId] = !currentStatus;

//         // เปลี่ยนรูปภาพ
//         const button = document.getElementById(lightId);
//         const img = button.querySelector("img");
//         img.src = lightStatuses[lightId] ? "img/light2.png" : "img/light.png";
//         button.title = lightStatuses[lightId] ? "light2" : "light1";

//         console.log(`สถานะไฟใหม่: ${lightId} = ${lightStatuses[lightId]}`);
//     } catch (error) {
//         console.error("ข้อผิดพลาดในการควบคุมไฟ:", error);
//     }
// }

// // เพิ่ม Event Listener ให้ปุ่มทั้งหมด
// document.querySelectorAll(".light-icon").forEach((button) => {
//     button.addEventListener("click", () => toggleLight(button.id));
// });

// // ดึงข้อมูลเซนเซอร์ทุก 3 วินาที
// setInterval(fetchSensorData, 3000);

// // เรียกฟังก์ชันดึงข้อมูลเซนเซอร์ครั้งแรก
// fetchSensorData();
