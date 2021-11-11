const fs = require('fs');
const zlib = require('zlib');

const directoryFiles = fs.readdirSync('./minified');

Promise.all(directoryFiles.map(filename => {
    if (filename.indexOf('.html')>=0) {
        return new Promise((resolve, reject) => {
            const fileContents = fs.createReadStream(`./minified/${filename}`);
            const writeStream = fs.createWriteStream(`./minified/${filename}.gz`);
            const gzip = zlib.createGzip();
            // const deflate = zlib.createDeflate();
            fileContents.pipe(gzip)/*.pipe(deflate)*/.pipe(writeStream).on('finish', (err) => {
                if (err) return reject(err);
                else resolve();
            })
        })
    }
}))
.then(console.log('done'));
