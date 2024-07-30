#include <Arduino.h>

#include <WiFi.h>
#include <GithubReleaseOTA.h>

#define WIFI_SSID WIFI_SSID
#define WIFI_PASS WIFI_PASS

#define GITHUB_OWNER GITHUB_OWNER
#define GITHUB_REPO GITHUB_REPO

GithubReleaseOTA ota(GITHUB_OWNER, GITHUB_REPO);

GithubRelease release;
bool readyForUpdate = false;

void setup() {
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("IP Address: " + WiFi.localIP().toString());

    // Get the latest release from GitHub
    std::vector<String> tags = ota.getReleaseTagList();

    if (tags.size() > 0) {
        // Print the release tags
        for (int i = 0; i < tags.size(); i++) {
            Serial.println(tags[i]);
        }

        release = ota.getReleaseByTagName(tags[0].c_str());
        if (release.tag_name != NULL) {
            Serial.println("Flash Firmware: " + String(release.tag_name));
            readyForUpdate = true;
        } else {
            Serial.println("Failed to get latest release");
            readyForUpdate = false;
        }
    }
}

void loop() {
    if (readyForUpdate) {
        int result = ota.flashFirmware(release, "firmware.bin");
        Serial.println("Flash firmware result: " + String(result));

        if (result == 0) {
            Serial.println("Firmware updated successfully");

            ESP.restart();
        } else {
            Serial.println("Firmware update failed: " + String(result));
            ota.freeRelease(release);
            readyForUpdate = false;
        }
    }
}

