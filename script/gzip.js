const fs = require('fs');

const { createGzip } = require('zlib');
const { pipeline } = require('stream');
const {
    createReadStream,
    createWriteStream
} = require('fs');

// const gzip = createGzip();
// const source = createReadStream('input.txt');
// const destination = createWriteStream('input.txt.gz');
//
// pipeline(source, gzip, destination, (err) => {
//     if (err) {
//         console.error('An error occurred:', err);
//         process.exitCode = 1;
//     }
// });

// Or, Promisified

const { promisify } = require('util');
const pipe = promisify(pipeline);

async function do_gzip(input, output) {
    const gzip = createGzip();
    const source = createReadStream(input);
    const destination = createWriteStream(output);
    await pipe(source, gzip, destination);
}

const directoryFiles = fs.readdirSync('./minified');

Promise.all(directoryFiles.map(filename => {
    if (filename.endsWith('.html')>=0) {
        return new Promise((resolve, reject) => {
            do_gzip(`./minified/${filename}`, `./minified/${filename}.gz`)
                .catch((err) => {
                    console.error('An error occurred:', err);
                });
        })
    }
}))
    .then(console.log('done'));

