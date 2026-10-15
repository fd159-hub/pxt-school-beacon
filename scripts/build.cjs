// Keep all MakeCode cache files inside this checkout, including in sandboxed Windows sessions.
const path = require('node:path');
const files = require('makecode-core/built/files.js');
const original = files.mkHomeCacheAsync;
files.mkHomeCacheAsync = dir => original(dir || path.resolve(__dirname, '../.pxt'));
require('makecode/built/cli.js');
