var express = require('express');
var router = express.Router();

const dataMap = new Map();

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

    if (dataMap.has(id)) {
        const existingData = dataMap.get(id);
        dataMap.set(id, { ...existingData, ip, state, saveState: saveState });
        return res.status(200).json({
            message: 'Eintrag aktualisiert!',
            data: { id, ip, state, saveState: saveState },
        });
    }

    dataMap.set(id, { ip, state, saveState: saveState });
    return res.status(201).json({
        message: 'Eintrag erstellt!',
        data: { id, ip, state, saveState: saveState },
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
    dataMap.set(id, { ...data, state });
    res.status(200).json({ message: 'State erfolgreich aktualisiert!', data: { id, state } });
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
    dataMap.set(id, { ...data, saveState: saveState, state: state });
    res.status(200).json({ message: 'SaveState erfolgreich aktualisiert!', data: { id, saveState } });
});

router.get('/ips', (req, res) => {
    const uniqueIps = new Set();

    dataMap.forEach((value) => {
        uniqueIps.add(value.ip);
    });

    res.status(200).json(Array.from(uniqueIps));
});

module.exports = router;