# ZonePad TS

This repository contains an ESPHome + LVGL configuration for a Guition ESP32-S3-4848S040 (480x480) touch display. It builds a zone control UI (zone levels, source selection, mic/music sliders) with shared assets and HTTP helpers to talk to an Optimal Audio Zone processor backend.
It is not formally supported by Optimal Audio, but uses public APIs and is very simple to adapt.

## Description
- This code adds a “Zone Audio” button to the example main page that navigates to a page of Zone audio controls for Zone 1.
- The Zone page shows source selection buttons and sliders for music/system mic 1 levels.
- HTTP helper (`http_client_helper.h`) fetches/sends zone values to `optimalaudio.local` with a configurable `zone_id`.
- Shared assets (SVG icons), colors, and scripts live in `zone_common.yaml`; per-zone layout is in `zone.yaml`.
- Fonts, themes, and hardware definitions are pulled in from the `esphome-modular-lvgl-buttons` package structure.

## Getting started
1) Install ESPHome:
   ```bash
   pip install esphome
   ```
   or use the ESPHome Dashboard/Docker if you prefer.

2) Ensure you have the ESP32 toolchain/Arduino support (ESPHome will prompt to install if missing).

3) Build/flash:
   ```bash
   esphome run guition-esp32-s3-4848s040-display_modular.yaml
   ```
   This compiles and flashes to your board (adjust the device/port as needed).

## Project structure
- `guition-esp32-s3-4848s040-display_modular.yaml` — main ESPHome entrypoint and package includes.
- `zone_common.yaml` — shared substitutions, assets (SVGs), scripts, and images.
- `zone.yaml` — per-zone layout and interval polling; uses `zone_id`, `zone_name`, `zone_prefix`.
- `http_client_helper.h` — simple C++ helpers to GET/SET values via HTTP API for a specific zone and parse the JSON response.
- `assets/` — SVG icons (music, mic, OA logos, including black variant).

## HTTP API examples
The helper targets `optimalaudio.local` and takes `zone_id` plus a parameter:
- Get current source:
  ```
  http://optimalaudio.local/api/get?zone=1&source
  ```
- Get a parameter value (e.g., music level):
  ```
  http://optimalaudio.local/api/get?zone=1&musiclevel
  ```
- Set a parameter (e.g., music level to 50):
  ```
  http://optimalaudio.local/api/set?zone=1&musiclevel=50
  ```
Replace `zone=1` with your configured `zone_id`. The helper functions in `http_client_helper.h` build these URLs automatically.
Further examples from Optimal Audio: https://support.optimal-audio.co.uk/hc/en-gb/articles/26753487856274-Zone-HTTP-control

## Configuring more zone pages
- Set `zone_id`, `zone_name`, and `zone_prefix` in `zone.yaml`. Duplicate the file with new values to add another zone, and include it from the main YAML as another package.
- Source labels/values are substitutions in `zone_common.yaml` (`source1_label/value`, etc.).

## Notes
- Icons are registered in `zone_common.yaml`; the main button uses `zone_audio_icon` (black OA on grey).
- Fonts rely on Nunito SemiBold included via `esphome-modular-lvgl-buttons/common/fonts.yaml`; only existing font IDs are used.

## Troubleshooting
- ID not found: ensure the page/widget IDs use the `${zone_prefix}` naming and that the correct page (`zone1_page`) is referenced.
- Duplicate IDs: avoid defining the same image/font in multiple includes; `zone_common.yaml` owns the SVG registrations.
- HTTP errors: confirm `optimalaudio.local` resolves on your network or adjust the hostname in `http_client_helper.h`.
