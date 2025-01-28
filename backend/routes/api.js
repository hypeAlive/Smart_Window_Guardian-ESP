var express = require('express');
var router = express.Router();
var espRouter = require('./esp');

router.get('/', function(req, res, next) {
  res.send("Window Guardian Public API");
});

router.use('/esp', espRouter);

module.exports = router;
