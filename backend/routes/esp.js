var express = require('express');
var router = express.Router();

const dataMap = new Map();
const testData = {
    ip: '127.0.0.1',
    state: 'UNKNOWN',
    saveState: 'UNKNOWN',
    timestamp: Date.now(),
};

setInterval(() => {
    const now = Date.now();
    dataMap.forEach((value, id) => {
        if (value.state === 'SETUP' && now - value.timestamp > 2 * 60 * 1000) {
            if(id === 'test') return;
            console.log(`Setze ID ${id} von SETUP auf UNKNOWN (zu lange inaktiv)`);
            dataMap.set(id, { ...value, state: 'UNKNOWN' });
        }
    });
}, 2 * 60 * 1000);

router.get('/', (req, res) => {
    const filteredEntries = Array.from(dataMap.entries())
        .filter(([id, value]) => value.state === 'SETUP')
        .map(([id, value]) => ({ id, ...value }));

    res.status(200).send(filteredEntries);
});

router.post('/register', (req, res) => {
    const { ip, id, state, saveState } = req.body;

    if (!ip || !id || !state) {
        return res.status(400).json({ error: 'IP, State und ID sind erforderlich!' });
    }

    const timestamp = Date.now();

    if (dataMap.has(id)) {
        const existingData = dataMap.get(id);
        dataMap.set(id, { ...existingData, ip, state, saveState, timestamp });
        return res.status(200).json({
            message: 'Eintrag aktualisiert!',
            data: { id, ip, state, saveState, timestamp },
        });
    }

    dataMap.set(id, { ip, state, saveState, timestamp });
    return res.status(201).json({
        message: 'Eintrag erstellt!',
        data: { id, ip, state, saveState, timestamp },
    });
});

router.post('/state', (req, res) => {
    const { id, state } = req.body;

    if (!id || !state) {
        return res.status(400).json({ error: 'ID und State sind erforderlich!' });
    }

    if (!dataMap.has(id)) {
        return res.status(404).json({ error: 'ID nicht gefunden!' });
    }

    const data = dataMap.get(id);
    const timestamp = Date.now();
    dataMap.set(id, { ...data, state, timestamp });
    res.status(200).json({ message: 'State erfolgreich aktualisiert!', data: { id, state, timestamp } });
});

router.post('/save-state', (req, res) => {
    const { id, saveState, state } = req.body;

    if (!id || saveState === undefined) {
        return res.status(400).json({ error: 'ID und saveState sind erforderlich!' });
    }

    if (!dataMap.has(id)) {
        return res.status(404).json({ error: 'ID nicht gefunden!' });
    }

    const data = dataMap.get(id);
    const timestamp = Date.now();
    dataMap.set(id, { ...data, saveState, state, timestamp });
    res.status(200).json({ message: 'SaveState erfolgreich aktualisiert!', data: { id, saveState, timestamp } });
});

router.post('/ping', (req, res) => {
    const { id } = req.body;

    if (!id) {
        return res.status(400).json({ error: 'ID ist erforderlich!' });
    }

    if (!dataMap.has(id)) {
        return res.status(404).json({ error: 'ID nicht gefunden!' });
    }

    const data = dataMap.get(id);
    const timestamp = Date.now();
    dataMap.set(id, { ...data, timestamp });
    res.status(200).json({ message: 'Ping erfolgreich!', data: { id, timestamp } });
});

router.get('/ips', (req, res) => {
    const uniqueIps = new Set();

    dataMap.forEach((value) => {
        uniqueIps.add(value.ip);
    });

    res.status(200).json(Array.from(uniqueIps));
});

module.exports = router;