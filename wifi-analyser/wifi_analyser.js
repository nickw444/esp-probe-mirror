const fs = require('fs');
const path = require('path');

const PIO_WARNING = `*****************************************************************************************************************************************************************************
Obsolete PIO Core v3.6.3 is used (previous was 4.0.3)
Please remove multiple PIO Cores from a system:
https://docs.platformio.org/page/faq.html#multiple-pio-cores-in-a-system
*****************************************************************************************************************************************************************************`


/**
 * TODO
 * SSID -> device ID mapping
 * MAC address vendor lookup
 */

function loadMacVendorLookup() {
  return new Map(fs.readFileSync('mac-vendor-2.txt', 'utf-8')
      .split('\n')
      .map(line => line.trim())
      .filter(line => line.length > 0)
      .map(line => line.split('\t')));
}

function loadData(file) {
  return fs.readFileSync(file, 'utf8')
      .replace(PIO_WARNING, '')
      .split('\n')
      .map(line => line.trim())
      .filter(line => line.length > 0)
      .map(line => line.split('\t'))
      .map(([ssid, channel, rate, rssi, srcAddr]) => ({
        ssid: ssid.trim(),
        channel: Number(channel),
        rate: Number(rate),
        rssi: Number(rssi),
        srcAddr: srcAddr && srcAddr.trim(),
      }));
}

function help() {
  console.log(`
    node ${path.basename(process.argv[1])} <data-file> <command>
    
    Available Commands:
      
      ssids:    Show SSIDs in order of number of probe frames seen
      ssiddev:  Show SSIDs and the MAC addresses that probed them
      devices:  Show device MAC addresses and the associated SSIDs that were probed
  `);
}


function ssids(data) {
  const ssidFreq = new Map();

  data
      .forEach(({ssid}) => {
        const count = ssidFreq.get(ssid) || 0;
        ssidFreq.set(ssid, count + 1);
      });

  Array.from(ssidFreq.entries())
      .map(([ssid, count]) => ({ssid, count}))
      .sort((a, b) => a.count - b.count)
      .reverse()
      .forEach(({ssid, count}) => {
        console.log(`${count}\t${ssid}`)
      });

  console.log(`\nTotal: ${ssidFreq.size}`)
}

function ssidDev(data, getVendor) {
  const deviceSsids = {};
  data.forEach(entry => {
    deviceSsids[entry.ssid] = deviceSsids[entry.ssid] || new Set();
    deviceSsids[entry.ssid].add(entry.srcAddr)
  });
  Object.entries(deviceSsids)
      .sort(([, devices1], [, devices2]) => devices1.size - devices2.size)
      .reverse()
      .forEach(([ssid, devices]) => {
        console.log(`SSID: ${ssid}`);
        Array.from(devices)
            .sort()
            .forEach(device => {
              const vendorName = getVendor(device);
              const vendorLabel = vendorName != null ? ` (${vendorName})` : '';
              console.log(`  • ${device}${vendorLabel}`);
            });
        console.log('')
      });
}

function devices(data, getVendor) {
  const ssidsByDevice = {};
  data.forEach(entry => {
    ssidsByDevice[entry.srcAddr] = ssidsByDevice[entry.srcAddr] || new Set();
    ssidsByDevice[entry.srcAddr].add(entry.ssid)
  });

  Object.entries(ssidsByDevice)
  // Strip out single-use src addresses as Apple have mitigated against
  // probe sniffing
      .filter(([, ssids]) => ssids.size > 1)
      .sort(([, ssids1], [, ssids2]) => ssids1.size - ssids2.size)
      .reverse()
      .forEach(([device, ssids]) => {
        const vendorName = getVendor(device);
        const vendorLabel = vendorName != null ? ` (${vendorName})` : '';
        console.log(`Device: ${device}${vendorLabel}`);
        Array.from(ssids)
            .sort()
            .forEach(ssid => {
              console.log(`  • ${ssid}`);
            });
        console.log('')
      });
}

function prefixes(data, getVendor) {
  const macAddressFreq = new Map();
  data.map(entry => entry.srcAddr.slice(0, 8))
      .forEach((prefix) => {
        const count = macAddressFreq.get(prefix) || 0;
        macAddressFreq.set(prefix, count + 1);
      });

  Array.from(macAddressFreq.entries())
      .sort(([, c1], [, c2]) => c1 - c2)
      .reverse()
      .forEach(([prefix, count]) => {
        const vendorName = getVendor(prefix);
        console.log(`${count}\t${prefix}\t${vendorName || ''}`);
      });
}

function getVendor(vendorMap, macAddress) {
  const prefix = macAddress.replace(/\:/g, '').slice(0, 6);
  return vendorMap.get(prefix);
}

function main() {
  const dataFilePath = process.argv[2];
  const command = process.argv[3];
  const vendorMap = loadMacVendorLookup();
  const getVendorImpl = (mac) => getVendor(vendorMap, mac);

  const data = loadData(dataFilePath);
  switch (command) {
    case 'ssids':
      return ssids(data);
    case 'ssiddev':
      return ssidDev(data, getVendorImpl);
    case 'devices':
      return devices(data, getVendorImpl);
    case 'prefixes':
      return prefixes(data, getVendorImpl);
    case 'help':
    default:
      help();
      process.exit(1);
      return;
  }
}

main();
