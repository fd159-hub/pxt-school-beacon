// Validate the actual Intel HEX artifact before handing it to a teacher for flashing.
const fs = require('node:fs');
const crypto = require('node:crypto');
const file = process.argv[2];
if (!file) throw new Error('Usage: node scripts/verify-hex.cjs path/to/file.hex');
const data = fs.readFileSync(file);
const lines = data.toString('utf8').trim().split(/\r?\n/);
let dataBytes = 0;
let eof = false;
for (const [index, line] of lines.entries()) {
    if (!/^:([0-9a-fA-F]{2})+$/.test(line)) throw new Error('Invalid HEX syntax at line ' + (index + 1));
    const record = Buffer.from(line.slice(1), 'hex');
    if (record.length !== record[0] + 5) throw new Error('Invalid byte count at line ' + (index + 1));
    if ((record.reduce((sum, byte) => sum + byte, 0) & 255) !== 0) throw new Error('Checksum error at line ' + (index + 1));
    if (eof) throw new Error('Records after EOF');
    if (record[3] === 0) dataBytes += record[0];
    if (record[3] === 1) eof = true;
}
if (!eof || !dataBytes) throw new Error('HEX missing EOF or firmware data');
console.log(JSON.stringify({file,records:lines.length,dataBytes,sha256:crypto.createHash('sha256').update(data).digest('hex')},null,2));
