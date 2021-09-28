const SerialPort = require('serialport')
const Delimiter = require('@serialport/parser-delimiter')
const fs = require("fs")

const serialport = new SerialPort('COM20', {
    baudRate: 9600
})

let data_points = []

const parser = serialport.pipe(new Delimiter({ delimiter: '\n' }))
parser.on('data', (data) => {
    data_points.push(data.toString().trim().split("\t").map(v => parseInt(v)))
})

process.on('SIGINT', () => {
    fs.writeFileSync("data_points.json", JSON.stringify(data_points), {encoding: "utf-8"})
    process.exit()
});
