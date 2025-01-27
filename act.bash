#!/bin/bash
.\act.exe -W .github/workflows/deploy.yml -P self-hosted=ghcr.io/catthehacker/ubuntu:act-latest --secret-file .secrets --var-file .variables